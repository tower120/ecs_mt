#include <tower120/ecs/impl/static_any_contiguous_container.hpp>
#include "test_utils.hpp"

using namespace tower120::ecs::impl;

struct data{
    int x;
    bool operator==(const data& other) const noexcept { return x == other.x; }
};

void test_ctr_copy(){
    any_vector any_vec{ std::vector<data>{} };
    auto& vec = any_vec.cast<std::vector<data>>();

    vec.push_back({1});
    vec.push_back({2});
    vec.push_back({3});

    auto any_vec2 = any_vec;
    REQUIRE(any_vec.cast<std::vector<data>>() == any_vec2.cast<std::vector<data>>());
}

void test_ctr_move(){
    any_vector any_vec{ std::vector<data>{} };
    auto& vec = any_vec.cast<std::vector<data>>();

    vec.push_back({1});
    vec.push_back({2});
    vec.push_back({3});

    auto any_vec2 = std::move(any_vec);
    REQUIRE(any_vec.cast<std::vector<data>>() != any_vec2.cast<std::vector<data>>());
    REQUIRE(any_vec.size() == 0);
    REQUIRE_EQUAL(any_vec2.cast<std::vector<data>>(), {data{1}, data{2}, data{3}});
}

int main() {
    any_vector any_vec{ std::vector<data>{} };
    auto& vec = any_vec.cast<std::vector<data>>();

    // unordered erase test
    REQUIRE(any_vec.size() == 0);
    vec.push_back({1});
    REQUIRE(any_vec.size() == 1);
    any_vec.unordered_erase(0);
    REQUIRE(any_vec.size() == 0);

    // TODO: unordered move test

    // container move test
    test_ctr_move();

    // container copy test
    test_ctr_copy();
}