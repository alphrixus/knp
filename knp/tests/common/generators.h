/**
 * @brief Common generators used for tests.
 * @author Artiom N.
 * @date 23.06.2023
 */

#pragma once

#include <knp/core/population.h>
#include <knp/core/projection.h>
#include <knp/neuron-traits/blifat.h>
#include <knp/synapse-traits/delta.h>

#include <optional>


/**
 * @brief Test namespace.
 */
namespace knp::testing
{
using DeltaProjection = knp::core::Projection<knp::synapse_traits::DeltaSynapse>;
using STDPDeltaProjection = knp::core::Projection<knp::synapse_traits::AdditiveSTDPDeltaSynapse>;
using BLIFATPopulation = knp::core::Population<knp::neuron_traits::BLIFATNeuron>;

// Create an input projection
inline std::optional<DeltaProjection::Synapse> input_projection_gen(size_t /*index*/)  // NOLINT
{
    return DeltaProjection::Synapse{{1.0, 1, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};
}


// Create an STDP input projection
inline std::optional<STDPDeltaProjection::Synapse> stdp_input_projection_gen(size_t /*index*/)  // NOLINT
{
    return STDPDeltaProjection::Synapse{
        {{.tau_plus_ = 2, .tau_minus_ = 2}, {1.0, 1, knp::synapse_traits::OutputType::EXCITATORY}}, 0, 0};
}


// Create a loop projection
inline std::optional<DeltaProjection::Synapse> synapse_generator(size_t /*index*/)  // NOLINT
{
    return DeltaProjection::Synapse{{1.0, 6, knp::synapse_traits::OutputType::EXCITATORY}, 0, 0};
}


// Create an STDP loop projection
inline std::optional<STDPDeltaProjection::Synapse> stdp_synapse_generator(size_t /*index*/)  // NOLINT
{
    return STDPDeltaProjection::Synapse{
        {{.tau_plus_ = 1, .tau_minus_ = 1}, {1.0, 6, knp::synapse_traits::OutputType::EXCITATORY}}, 0, 0};
}


// Create population
inline knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron> neuron_generator(size_t)  // NOLINT
{
    return knp::neuron_traits::neuron_parameters<knp::neuron_traits::BLIFATNeuron>{};
}
}  // namespace knp::testing
