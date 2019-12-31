#include <tower120/ecs/impl/static_any_contiguous_container.hpp>
#include "test_utils.hpp"

struct data{
    int x;
};

int main() {
    using namespace tower120::ecs::impl;

    any_vector any_vec{ std::vector<data>{} };
    auto& vec = any_vec.cast<std::vector<data>>();

    REQUIRE(any_vec.size() == 0);
    vec.push_back({1});
    REQUIRE(any_vec.size() == 1);
    any_vec.unordered_erase(0);
    REQUIRE(any_vec.size() == 0);

    // move test
    vec.push_back({2});
    auto any_vec2{std::move(any_vec)};
    //auto any_vec2{std::move(any_vec)};
    REQUIRE(any_vec.size()  == 0);
    REQUIRE(any_vec2.size() == 1);
}