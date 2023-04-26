/**
 * @file message_envelope.h
 * @brief Message envelope routines.
 * @author Artiom N.
 * @date 13.04.2023
 */

#pragma once

#include <knp/core/uid.h>

#include <iostream>
#include <variant>
#include <vector>

#include "messaging.h"


/**
 * @brief Messaging namespace.
 */
namespace knp::core::messaging
{

/**
* @brief Message variant that contains any message type specified in `AllMessages`.
* @details `MessageVariant` takes the value of `std::variant<MessageType_1,..., MessageType_n>`, where `MessageType_[1..n]` is the message type specified in `AllMessages`. 
*       For example, if `AllMessages` containes SpikeMessage and SynapticImpactMessage types, then `MessageVariant = std::variant<SpikeMessage, SynapticImpactMessage>`. 
*       `MessageVariant` retains the same order of message types as defined in `AllMessages`.
* @see ALL_MESSAGES.
*/
using MessageVariant = boost::mp11::mp_rename<AllMessages, std::variant>;

std::vector<uint8_t> pack_to_envelope(const MessageVariant &message);
MessageVariant extract_from_envelope(const void *buffer);
MessageVariant extract_from_envelope(std::vector<uint8_t> &buffer);

}  // namespace knp::core::messaging
