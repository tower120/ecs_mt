#include <tower120/ecs/impl/any_contiguous_array.hpp>
#include "test_utils.hpp"

struct data{
    int x;
};

int main() {
    using namespace tower120::ecs::impl;

    any_contiguous_array any_vec{ std::vector<data>{} };
    auto& vec = any_vec.cast<std::vector<data>>();

    REQUIRE(any_vec.size() == 0);
    vec.push_back({1});
    REQUIRE(any_vec.size() == 1);
    any_vec.unordered_erase(0);
    REQUIRE(any_vec.size() == 0);

}