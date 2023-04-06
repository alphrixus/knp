/**
 * @file subscription.h
 * @brief defines Subscription, a class that determines message exchange between entities in the network
 * @author Vartenkov A.
 * @date 15.03.2023
 */

#pragma once
#include <knp/core/uid.h>

#include <algorithm>
#include <string>
#include <unordered_set>
#include <vector>

#include <boost/functional/hash.hpp>


namespace knp::core
{

/**
 * @brief A subscription class used for message exchange among network entities
 * @tparam T type of messages
 */
template <class T>
class Subscription
{
public:
    using MessageType = T;
    using MessageContainerType = std::vector<MessageType>;

public:
    Subscription(const UID &reciever, const std::vector<UID> &senders) : receiver_(reciever) { add_senders(senders); }

    /// Get a set of sender UIDs
    [[nodiscard]] const auto &get_senders() const { return senders_; }

    /// Get receiver UID
    [[nodiscard]] UID get_receiver_uid() const { return receiver_; }

    /**
     * @brief Unsubscribe from a sender. If not subscribed to the sender, do nothing.
     * @param uid sender UID
     * @return 1 if unsubscribed, 0 if no sender was found
     */
    size_t remove_sender(const UID &uid) { return senders_.erase(static_cast<std::string>(uid)); }

    /**
     * @brief Add an additional sender to the subscription
     * @param uid new sender UID
     * @return number of senders added (0 if already subscribed, 1 otherwise)
     */
    size_t add_sender(const UID &uid) { return senders_.insert(static_cast<std::string>(uid)).second; }

    /**
     * @brief Add a number of senders to the subscription
     * @param senders a vector of sender UIDs
     * @return number of new senders added
     */
    size_t add_senders(const std::vector<UID> &senders)
    {
        // size_t size_before = senders_.size();
        // std::copy(senders.begin(), senders.end(), std::inserter(senders_, senders_.end()));
        //        return senders_.size() - size_before;
        return 0;
    };

    /**
     * @brief Checks if a sender exists
     * @param uid sender UID
     * @return true if sender exists
     */
    [[nodiscard]] bool has_sender(const UID &uid) const
    {
        return senders_.find(static_cast<std::string>(uid)) != senders_.end();
    }

public:
    void add_message(MessageType &&message) { messages_.push_back(message); }
    void add_message(const MessageType &message) { messages_.emplace_back(message); }

    MessageContainerType &get_messages() { return messages_; }
    const MessageContainerType &get_messages() const { return messages_; }

private:
    /// Receiver UID
    const UID receiver_;
    /// Set of sender UIDs
    // boost::hash<UID>.
    std::unordered_set<std::string> senders_;
    /// Message cache
    MessageContainerType messages_;
};


}  // namespace knp::core
