#include <tower120/ecs/impl/epoch_hierarchical_container.hpp>
#include "test_utils.hpp"

using namespace tower120::ecs::impl;

int main(){
    // 0 1 2 3 4 5 6 7
    // 1   3   5   7
    // 3       7

    epoch_hierarchical_container<std::size_t> container{1, 3};

    // test push_back
    container.push_back(0);
    container.push_back(1);
    container.push_back(2);
    container.push_back(3);
    container.push_back(4);
    container.push_back(5);
    container.push_back(6);
    container.push_back(7);

    REQUIRE_EQUAL(container.levels()[0], {0, 1, 2, 3, 4, 5, 6, 7});
    REQUIRE_EQUAL(container.levels()[1], {1, 3, 5, 7});
    REQUIRE_EQUAL(container.levels()[2], {3, 7});

    // XXX: NOT WORK. SMTHG WRONG
    // test erase at the end
    {
        auto c = container;

        c.unordered_erase(7);
        REQUIRE_EQUAL(c.levels()[0], {0, 1, 2, 3, 4, 5, 6});
        REQUIRE_EQUAL(c.levels()[1], {1, 3, 5, 6});
        REQUIRE_EQUAL(c.levels()[2], {3, 6});

        c.unordered_erase(6);
        REQUIRE_EQUAL(c.levels()[0], {0, 1, 2, 3, 4, 5});
        REQUIRE_EQUAL(c.levels()[1], {1, 3, 5});
        REQUIRE_EQUAL(c.levels()[2], {3, 5});


        /*c.unordered_erase(1);
        REQUIRE_EQUAL(c.levels()[0], {0, 5, 2, 3, 4});
        REQUIRE_EQUAL(c.levels()[1], {5, 3, });
        REQUIRE_EQUAL(c.levels()[2], {3});*/

    }
};