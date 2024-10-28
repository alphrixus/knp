/**
 * @file inference.h
 * @brief Implementing inference for MNIST example network.
 * @date 18.09.2024
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#pragma once
#include <filesystem>
#include <vector>


/**
 * @brief Structure that stores inference results from a single population.
 */
struct InferenceResult
{
    /**
     * @brief Response creation step.
     */
    size_t step_ = 0;

    /**
     * @brief Indexes of spiking neurons.
     */
    std::vector<int> indexes_{};
};


/**
 * @brief Run inference for MNIST model.
 * @param path_to_model path to SONATA model directory.
 * @param path_to_data path to binary data file.
 * @param path_to_backend path to backend.
 * @return vector of inference results.
 */
std::vector<InferenceResult> do_inference(
    const std::filesystem::path &path_to_model, const std::filesystem::path &path_to_data,
    const std::filesystem::path &path_to_backend);
