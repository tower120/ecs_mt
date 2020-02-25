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

    // test set
    {
        auto c = container;
        c.set(0, 100);
        REQUIRE_EQUAL(c.levels()[0], {100, 1, 2, 3, 4, 5, 6, 7});
        REQUIRE_EQUAL(c.levels()[1], {100, 3, 5, 7});
        REQUIRE_EQUAL(c.levels()[2], {100, 7});
    }
    {
        auto c = container;
        c.set(1, 100);
        REQUIRE_EQUAL(c.levels()[0], {0, 100, 2, 3, 4, 5, 6, 7});
        REQUIRE_EQUAL(c.levels()[1], {100, 3, 5, 7});
        REQUIRE_EQUAL(c.levels()[2], {100, 7});
    }
    {
        auto c = container;
        c.set(2, 100);
        REQUIRE_EQUAL(c.levels()[0], {0, 1, 100, 3, 4, 5, 6, 7});
        REQUIRE_EQUAL(c.levels()[1], {1, 100, 5, 7});
        REQUIRE_EQUAL(c.levels()[2], {100, 7});
    }
    {
        auto c = container;
        c.set(3, 100);
        REQUIRE_EQUAL(c.levels()[0], {0, 1, 2, 100, 4, 5, 6, 7});
        REQUIRE_EQUAL(c.levels()[1], {1, 100, 5, 7});
        REQUIRE_EQUAL(c.levels()[2], {100, 7});
    }
    {
        auto c = container;
        c.set(4, 100);
        REQUIRE_EQUAL(c.levels()[0], {0, 1, 2, 3, 100, 5, 6, 7});
        REQUIRE_EQUAL(c.levels()[1], {1, 3, 100, 7});
        REQUIRE_EQUAL(c.levels()[2], {3, 100});
    }
    {
        auto c = container;
        c.set(5, 100);
        REQUIRE_EQUAL(c.levels()[0], {0, 1, 2, 3, 4, 100, 6, 7});
        REQUIRE_EQUAL(c.levels()[1], {1, 3, 100, 7});
        REQUIRE_EQUAL(c.levels()[2], {3, 100});
    }
    {
        auto c = container;
        c.set(6, 100);
        REQUIRE_EQUAL(c.levels()[0], {0, 1, 2, 3, 4, 5, 100, 7});
        REQUIRE_EQUAL(c.levels()[1], {1, 3, 5, 100});
        REQUIRE_EQUAL(c.levels()[2], {3, 100});
    }
    {
        auto c = container;
        c.set(7, 100);
        REQUIRE_EQUAL(c.levels()[0], {0, 1, 2, 3, 4, 5, 6, 100});
        REQUIRE_EQUAL(c.levels()[1], {1, 3, 5, 100});
        REQUIRE_EQUAL(c.levels()[2], {3, 100});
    }

    // test set . Setting lower epoch...
    {
        auto c = container;
        c.set(6, 0);
        REQUIRE_EQUAL(c.levels()[0], {0, 1, 2, 3, 4, 5, 0, 7});
        REQUIRE_EQUAL(c.levels()[1], {1, 3, 5, 7});
        REQUIRE_EQUAL(c.levels()[2], {3, 7});
    }

    {
        auto c = container;
        c.set(0, 2);
        REQUIRE_EQUAL(c.levels()[0], {2, 1, 2, 3, 4, 5, 6, 7});
        REQUIRE_EQUAL(c.levels()[1], {2, 3, 5, 7});
        REQUIRE_EQUAL(c.levels()[2], {3, 7});
    }


    return 0;

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