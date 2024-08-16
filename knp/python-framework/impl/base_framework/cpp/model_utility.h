//
// Created by an_vartenkov on 14.06.24.
//

#pragma once
#include <knp/framework/model.h>

#include <memory>
#include <utility>


std::shared_ptr<knp::framework::Model> model_constructor(knp::framework::Network &network)
{
    knp::framework::Model model{std::move(network)};
    return std::make_shared<knp::framework::Model>(std::move(model));
}


// A very bad and wrong way to access network from model
knp::framework::Network get_network_from_model(knp::framework::Model &self)
{
    return self.get_network();
}


void set_network_to_model(knp::framework::Model &self, knp::framework::Network &network)
{
    self.get_network() = std::move(network);
}


auto get_model_input_channels(knp::framework::Model &self)
{
    return self.get_input_channels();
}


auto get_model_output_channels(knp::framework::Model &self)
{
    return self.get_output_channels();
}
