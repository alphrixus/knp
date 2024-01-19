/**
 * @file additive_stdp_impl.h
 * @brief Additive STDP calculation routines implementation.
 * @author Artiom N.
 * @date 21.08.2023
 */
#pragma once
#include <knp/backends/cpu-library/impl/base_stdp_impl.h>
#include <knp/core/message_endpoint.h>
#include <knp/core/messaging/messaging.h>
#include <knp/core/messaging/synaptic_impact_message.h>
#include <knp/core/projection.h>
#include <knp/synapse-traits/stdp_common.h>

#include <spdlog/spdlog.h>

#include <unordered_map>
#include <utility>
#include <vector>


namespace knp::backends::cpu
{

using knp::core::UID;
using knp::core::messaging::SpikeMessage;


const knp::synapse_traits::synapse_parameters<knp::synapse_traits::DeltaSynapse> &get_delta_synapse_params(
    const knp::synapse_traits::synapse_parameters<
        knp::synapse_traits::STDP<knp::synapse_traits::STDPAdditiveRule, knp::synapse_traits::DeltaSynapse>>
        &synapse_params)
{
    return synapse_params;
}


class STDPFormula
{
public:
    STDPFormula(float tau_plus, float tau_minus, float a_plus, float a_minus)
        : tau_plus_(tau_plus), tau_minus_(tau_minus), a_plus_(a_plus), a_minus_(a_minus)
    {
    }

    [[nodiscard]] float stdp_w(float weight_diff)
    {
        // Zhang et al. 1998.
        return weight_diff > 0 ? a_plus_ * std::exp(-weight_diff / tau_plus_)
                               : a_minus_ * std::exp(weight_diff / tau_minus_);
    }

    [[nodiscard]] float stdp_delta_w(
        const std::vector<uint32_t> &presynaptic_spikes, const std::vector<uint32_t> &postsynaptic_spikes)
    {
        // Gerstner and al. 1996, Kempter et al. 1999.

        assert(presynaptic_spikes.size() == postsynaptic_spikes.size());

        float w_j = 0;

        for (const auto &t_f : presynaptic_spikes)
            for (const auto &t_n : postsynaptic_spikes)
            {
                // cppcheck-suppress useStlAlgorithm
                w_j += stdp_w(t_n - t_f);
            }
        return w_j;
    }

    [[nodiscard]] float operator()(
        const std::vector<uint32_t> &presynaptic_spikes, const std::vector<uint32_t> &postsynaptic_spikes)
    {
        return stdp_delta_w(presynaptic_spikes, postsynaptic_spikes);
    }

private:
    float tau_plus_;
    float tau_minus_;
    float a_plus_;
    float a_minus_;
};


template <class DeltaLikeSynapse>
void append_spike_times(
    knp::core::Projection<knp::synapse_traits::AdditiveSTDPDeltaSynapse> &projection, const SpikeMessage &message,
    const std::function<std::vector<size_t>(uint32_t)> &synapse_index_getter,
    std::vector<uint32_t> knp::synapse_traits::STDPAdditiveRule<DeltaLikeSynapse>::*spike_queue)
{
    // Filling synapses spike queue.
    for (auto neuron_index : message.neuron_indexes_)
    {
        // Might be able to change it into "traces"
        // TODO: Inefficient, MUST be cached.
        for (auto synapse_index : synapse_index_getter(neuron_index))
        {
            auto &rule = projection[synapse_index].params_.rule_;
            // Limit spike times queue.
            if ((rule.*spike_queue).size() < rule.tau_minus_ + rule.tau_plus_)
                (rule.*spike_queue).push_back(message.header_.send_time_);
        }
    }
}


void append_spike_times(
    knp::core::Projection<knp::synapse_traits::AdditiveSTDPDeltaSynapse> &projection,
    const std::vector<SpikeMessage> &spikes, const std::function<std::vector<size_t>(uint32_t)> &syn_index_getter,
    std::vector<uint32_t> knp::synapse_traits::STDPAdditiveRule<knp::synapse_traits::DeltaSynapse>::*spike_queue)
{
    for (const auto &msg : spikes)
    {
        append_spike_times(projection, msg, syn_index_getter, spike_queue);
    }
}


template <class SynapseType>
constexpr bool is_additive_stdp_synapse()
{
    if constexpr (!std::is_same_v<
                      SynapseType, synapse_traits::STDP<synapse_traits::STDPAdditiveRule, typename SynapseType::Base>>)
        return is_additive_stdp_synapse<typename SynapseType::Base>();

    using AddSTDPProj =
        typename knp::core::Projection<synapse_traits::STDP<synapse_traits::STDPAdditiveRule, SynapseType>>;
    return std::is_same<std::decay_t<SynapseType>, AddSTDPProj>::value;
}


template <class DeltaLikeSynapse>
void register_additive_stdp_spikes(
    knp::core::Projection<knp::synapse_traits::STDP<knp::synapse_traits::STDPAdditiveRule, DeltaLikeSynapse>>
        &projection,
    std::vector<SpikeMessage> &all_messages)
{
    SPDLOG_DEBUG("Calculating Additive STDP Delta synapse projection");

    using ProjectionType = typename std::decay_t<decltype(projection)>;
    using ProcessingType = typename ProjectionType::SharedSynapseParameters::ProcessingType;

    const auto &stdp_pops = projection.get_shared_parameters().stdp_populations_;

    // Spike messages to process as usual.
    std::vector<SpikeMessage> stdp_only_messages;
    stdp_only_messages.reserve(all_messages.size());

    // TODO: Remove cycles.
    for (auto &msg : all_messages)
    {
        const auto &stdp_pop_iter = stdp_pops.find(msg.header_.sender_uid_);
        if (stdp_pop_iter == stdp_pops.end()) continue;

        const auto &[uid, processing_type] = *stdp_pop_iter;
        assert(uid == msg.header_.sender_uid_);
        if (processing_type == ProcessingType::STDPOnly || processing_type == ProcessingType::STDPAndSpike)
        {
            SPDLOG_TRACE("Add spikes to STDP projection postsynaptic history");
            append_spike_times(
                projection, msg,
                [&projection](uint32_t neuron_index) { return projection.get_by_postsynaptic_neuron(neuron_index); },
                &knp::synapse_traits::STDPAdditiveRule<knp::synapse_traits::DeltaSynapse>::postsynaptic_spike_times_);
        }
        if (processing_type == ProcessingType::STDPAndSpike)
        {
            SPDLOG_TRACE("Add spikes to STDP projection presynaptic history");
            append_spike_times(
                projection, msg,
                [&projection](uint32_t neuron_index) { return projection.get_by_postsynaptic_neuron(neuron_index); },
                &knp::synapse_traits::STDPAdditiveRule<knp::synapse_traits::DeltaSynapse>::presynaptic_spike_times_);
        }
        if (processing_type == ProcessingType::STDPOnly)
        {
            SPDLOG_TRACE("STDP only synapse, remove message from list");
            msg.neuron_indexes_ = {};
        }

        assert(processing_type == ProcessingType::STDPAndSpike || processing_type == ProcessingType::STDPOnly);
    }
}


template <class DeltaLikeSynapse>
void update_projection_weights_additive_stdp(
    knp::core::Projection<knp::synapse_traits::STDP<knp::synapse_traits::STDPAdditiveRule, DeltaLikeSynapse>>
        &projection)
{
    // Update projection parameters
    for (auto &s : projection)
    {
        SPDLOG_TRACE("Applying STDP rule...");
        auto &rule = s.params_.rule_;
        const auto period = rule.tau_plus_ + rule.tau_minus_;

        if (rule.presynaptic_spike_times_.size() >= period && rule.postsynaptic_spike_times_.size() >= period)
        {
            STDPFormula stdp_formula(rule.tau_plus_, rule.tau_minus_, 1, 1);
            SPDLOG_TRACE("Old weight = {}", s.params_.weight_);
            s.params_.weight_ += stdp_formula(rule.presynaptic_spike_times_, rule.postsynaptic_spike_times_);
            SPDLOG_TRACE("New weight = {}", s.params_.weight_);
            rule.presynaptic_spike_times_.clear();
            rule.postsynaptic_spike_times_.clear();
        }
    }
}


template <class DeltaLikeSynapse>
struct WeightUpdateSTDP<synapse_traits::STDP<synapse_traits::STDPAdditiveRule, DeltaLikeSynapse>>
{
    using Synapse = synapse_traits::STDP<synapse_traits::STDPAdditiveRule, DeltaLikeSynapse>;
    static void init_projection(
        knp::core::Projection<Synapse> &projection, std::vector<SpikeMessage> &all_messages, uint64_t step)
    {
        register_additive_stdp_spikes(projection, all_messages);
    }

    static void init_synapse(knp::synapse_traits::synapse_parameters<Synapse> &projection, uint64_t step) {}

    static void modify_weights(knp::core::Projection<Synapse> &projection)
    {
        update_projection_weights_additive_stdp(projection);
    }
};


}  // namespace knp::backends::cpu