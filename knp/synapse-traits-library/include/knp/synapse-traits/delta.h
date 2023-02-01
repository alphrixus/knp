/**
 * @file delta.h
 * @brief Delta synapse types traits.
 * @author Andrey V.
 * @date 26.01.2023
 */

#pragma once

#include "type_traits.h"
#include <cstdlib>

namespace knp::synapse_traits
{
    struct DeltaSynapse;

    template <>
    struct synapse_parameters<DeltaSynapse>
    {
        size_t pre_id;
        size_t post_id;
        float weight_;
        size_t delay_;
    };
}  // namespace knp::synapse_traits