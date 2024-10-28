/**
 * @file core_test.cpp.
 * @brief UUID tests.
 * @author Artiom N.
 * @date 21.08.2023.
 * @license Apache 2.0
 * @copyright © 2024 AO Kaspersky Lab
 */

#include <knp/core/core.h>

#include <tests_common.h>


TEST(CoreSuite, TagMapTest)
{
    knp::core::TagMap tag_map;

    tag_map.get_tag("test") = std::string("test_val");

    ASSERT_EQ(tag_map.get_tag<std::string>("test"), "test_val");

    tag_map["test"] = std::string("new");

    ASSERT_EQ(tag_map.get_tag<std::string>("test"), "new");
}
