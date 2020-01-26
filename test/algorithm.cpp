#include <tower120/ecs/impl/utils/algorithm.hpp>
#include "test_utils.hpp"

#include <vector>
#include <array>
#include <iterator>


void test_set_to_set_map(){
    using namespace tower120::ecs::impl::utils;

    std::vector<int> list1 = {10,20,30,40,50};
    std::vector<int> list2 = {20,40,50};

    using Iterator1 = decltype(list1)::iterator;
    using Iterator2 = decltype(list2)::iterator;

    std::vector<std::pair<Iterator1, Iterator2>> out;

    set_to_set_map(list1, list2, std::back_inserter(out));

    REQUIRE_EQUAL(out, {
        std::make_pair(list1.begin()+1, list2.begin()),
        std::make_pair(list1.begin()+3, list2.begin()+1),
        std::make_pair(list1.begin()+4, list2.begin()+2),
    });
}


int main(){
    test_set_to_set_map();
    return 0;
}