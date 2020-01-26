#include <tower120/ecs/impl/utils/output_iterator.hpp>
#include "test_utils.hpp"

#include <vector>
#include <array>
#include <iterator>
#include <algorithm>


int main(){
    using namespace tower120::ecs::impl::utils;

    std::vector<int> list1 = {10,20,30,40,50};
    std::vector<int> list2 = {20,45,50};
    std::vector<int> out;


    std::set_intersection(
        list1.begin(), list1.end(),
        list2.begin(), list2.end(),
        output_iterator([&out](int i){
            out.emplace_back(i);
        }));

    REQUIRE_EQUAL(out, {20, 50});
    return 0;
}