#include <tower120/ecs/impl/components_container.hpp>
#include "test_utils.hpp"

int main() {
    using namespace tower120::ecs;
    using namespace tower120::ecs::impl;
    //using namespace ranges;

    struct data_x {
       int x;

       bool operator==(const data_x& other) const{ return x == other.x; }
    };
    struct data_y {
        int y;

        bool operator==(const data_y& other) const{ return y == other.y; }
    };

    entity_manager entity_data_manager;
    using Archetype = archetype_t<data_x, data_y>;
    components_container container{entity_data_manager, Archetype::archetype};

    entity entity = entity_data_manager.make();

    // test emplace
    container.emplace(
        entity,
        data_x{1}, data_y{2}
    );

    // test per component entity access
    {
        const data_x& x1 = container.component<data_x>(entity);
        const data_x& x2 = container.component<data_x, Archetype>(entity);
        REQUIRE(x1 == x2);
        REQUIRE(x1.x == 1);

        const data_y& y1 = container.component<data_y>(entity);
        const data_y& y2 = container.component<data_y, Archetype>(entity);
        REQUIRE(y1 == y2);
        REQUIRE(y1.y == 2);
    }

    // test components access
    {
        const auto& xs1 = container.components<data_x, Archetype>();
        const auto& xs2 = container.components<data_x>();
        REQUIRE(xs1 == xs2);
        REQUIRE(xs1.size() == 1);
        REQUIRE(xs1[0].x == 1);
    }

    // test erase
    container.erase(entity);
    REQUIRE(container.size() == 0);

    {
        class entity old_entity = entity;

        // test entity free
        entity_data_manager.free(entity);

        // test entity reuse
        entity = entity_data_manager.make();

        REQUIRE(entity == old_entity);
    }

    // test erase w Archetype
    container.emplace(
        entity,
        data_x{1}, data_y{2}
    );
    container.erase<Archetype>(entity);
    REQUIRE(container.size() == 0);

    return 0;
}
