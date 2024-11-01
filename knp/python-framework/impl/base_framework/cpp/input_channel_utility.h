/**
 * @file input_channel_utility.h
 * @brief Auxiliary functions for input channel bindings.
 * @kaspersky_support Vartenkov A.
 * @date 05.06.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#pragma once
#include <knp/framework/io/input_channel.h>

#include <memory>
#include <utility>


std::shared_ptr<knp::framework::io::input::InputChannel> construct_input_channel(
    const knp::core::UID &uid, knp::core::MessageEndpoint &endpoint, knp::framework::io::input::DataGenerator &gen)
{
    return std::make_shared<knp::framework::io::input::InputChannel>(uid, std::move(endpoint), std::move(gen));
}


knp::core::UID get_input_channel_uid(knp::framework::io::input::InputChannel &self)
{
    return self.get_uid();
}
