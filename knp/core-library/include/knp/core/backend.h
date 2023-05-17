/**
 * @file backend.h
 * @brief Backend base class definition.
 * @author Artiom N.
 * @date 11.01.2023
 */

#pragma once

#include <knp/core/core.h>
#include <knp/core/device.h>
#include <knp/core/message_bus.h>

#include <atomic>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <boost/config.hpp>

/**
 * @brief Core library namespace.
 */
namespace knp::core
{

/**
 * @brief The Backend class is the base class for backends.
 */
class BOOST_SYMBOL_VISIBLE Backend
{
public:
    virtual ~Backend() = 0;

public:
    /**
     * @brief Get the backend UID.
     * @return backend UID.
     */
    [[nodiscard]] const UID &get_uid() const { return base_.uid_; }
    /**
     * @brief Get tags used by the backend.
     * @return backend tag map.
     * @see TagMap.
     */
    [[nodiscard]] auto &get_tags() { return base_.tags_; }

public:
    /**
     * @brief plasticity_supported true if plasticity supported.
     * @return plasticiy supported flag.
     */
    [[nodiscard]] virtual bool plasticity_supported() const = 0;
    /**
     * @brief get_supported_neurons return supported neurons names.
     * @return vector with supported neurons names.
     */
    [[nodiscard]] virtual std::vector<std::string> get_supported_neurons() const = 0;
    /**
     * @brief get_supported_synapses return supported synapses names.
     * @return vector with supported synapses names.
     */
    [[nodiscard]] virtual std::vector<std::string> get_supported_synapses() const = 0;

public:
    /**
     * @brief Remove projections with given UIDs from the backend.
     * @param uids UIDs of projections to remove.
     */
    virtual void remove_projections(const std::vector<UID> &uids) = 0;

    /**
     * @brief Remove populations with given UIDs from the backend.
     * @param uids UIDs of populations to remove.
     */
    virtual void remove_populations(const std::vector<UID> &uids) = 0;

public:
    /**
     * @brief Get a list of devices supported by the backend.
     * @return list of devices.
     * @see Device.
     */
    [[nodiscard]] virtual std::vector<std::unique_ptr<Device>> get_devices() const = 0;

    /**
     * @brief Get a list of devices on which the backend runs a network.
     * @return list of devices.
     * @see Device.
     */
    std::vector<std::unique_ptr<Device>> &get_current_devices() { return devices_; }
    const std::vector<std::unique_ptr<Device>> &get_current_devices() const { return devices_; }

    /**
     * @brief Select devices on which to run the backend.
     * @param uids set of device UIDs that the backend uses.
     */
    virtual void select_devices(const std::set<UID> &uids);

public:
    /**
     * @brief Start network execution on the backend.
     */
    void start();

    /**
     * @brief Stop network execution on the backend.
     */
    void stop();

    /**
     * @brief Make one network execution step.
     * @details You can use this method for debugging purposes.
     */
    virtual void step() = 0;

public:
    bool running() const { return started_; }

protected:
    /**
     * @brief Init network before starting.
     */
    virtual void init() = 0;
    void uninit();

public:
    MessageBus message_bus_;

private:
    const BaseData base_;
    std::atomic<bool> initialized_ = false;
    std::atomic<bool> started_ = false;
    std::vector<std::unique_ptr<Device>> devices_;
};

}  // namespace knp::core
