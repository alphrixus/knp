/**
 * @file observer.h
 * @brief an observer class that unloads messages and stores them.
 * @author Vartenkov A.
 * @date 23.08.2023
 */
#pragma once
#include <knp/core/message_endpoint.h>
#include <knp/core/messaging/messaging.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/mp11.hpp>


namespace knp::monitoring
{
/**
 * @brief Functor for message processing.
 */
template <class Message>
using MessageProcessor = std::function<void(const std::vector<Message>)>;


/**
 * @brief A class that receives messages and process them.
 * @tparam Message message type that is processed by an observer.
 * @note use this for statistics calculation or for information output.
 */
template <class Message>
class MessageObserver
{
public:
    /**
     * @brief Constructor.
     * @param endpoint endpoint to get messages from.
     * @param processor functor to process messages.
     * @param uid observer uid.
     */
    MessageObserver(
        core::MessageEndpoint &&endpoint, MessageProcessor<Message> &&processor, core::UID uid = core::UID{true})
        : endpoint_(std::move(endpoint)), process_messages_(processor), uid_(uid)
    {
    }

    /**
     * @brief Subscribe to messages.
     * @param entities message senders.
     */
    void subscribe(const std::vector<core::UID> &entities) { endpoint_.subscribe<Message>(uid_, entities); }

    /**
     * @brief Receive and process messages.
     */
    void update()
    {
        endpoint_.receive_all_messages();
        auto messages_raw = endpoint_.unload_messages<Message>(uid_);
        process_messages_(messages_raw);
    }

private:
    core::MessageEndpoint endpoint_;
    MessageProcessor<Message> process_messages_;
    core::UID uid_;
};

/**
 * @brief list of all possible observers.
 */
using AllObservers = boost::mp11::mp_transform<MessageObserver, core::messaging::AllMessages>;

/**
 * @brief observer variant, containing all possible observers.
 */
using AnyObserverVariant = boost::mp11::mp_rename<AllObservers, std::variant>;

}  // namespace knp::monitoring
