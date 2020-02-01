#include <tower120/ecs/impl/components_container.hpp>
#include "test_utils.hpp"

using namespace tower120::ecs;
using namespace tower120::ecs::impl;


struct data_x {
   int x;
   bool operator==(const data_x& other) const{ return x == other.x; }
};
struct data_y {
    int y;
    bool operator==(const data_y& other) const{ return y == other.y; }
};
struct data_z {
    int z;
    bool operator==(const data_z& other) const{ return z == other.z; }
};
struct data_w {
    int w;
    bool operator==(const data_w& other) const{ return w == other.w; }
};


void test_inter_container_entity_move(){
    entity_manager entity_data_manager;
    using ArchetypeXY  = archetype<data_x, data_y>;
    using ArchetypeXYZ = archetype<data_x, data_y, data_z>;

    entity entity1 = entity_data_manager.make();
    entity entity2 = entity_data_manager.make();
    entity entity3 = entity_data_manager.make();

    // init
    components_container container_xy{ArchetypeXY::type};
    container_xy.emplace(
        entity1,
        data_x{11}, data_y{21}
    );
    container_xy.emplace(
        entity2,
        data_x{12}, data_y{22}
    );

    // emplace
    components_container container_xyz{ArchetypeXYZ::type};
    container_xyz.emplace(
        entity3,
        data_x{13}, data_y{23}, data_z{33}
    );

    container_xy.move_entity(container_xyz, entity1, data_z{31});
    REQUIRE_SET_EQUAL(container_xy.entities(),  {entity2});
    REQUIRE_SET_EQUAL(container_xyz.entities(), {entity3, entity1});
    REQUIRE_SET_EQUAL(container_xyz.components<data_x>(), {data_x{11}, data_x{13}});
    REQUIRE_SET_EQUAL(container_xyz.components<data_y>(), {data_y{21}, data_y{23}});
    REQUIRE_SET_EQUAL(container_xyz.components<data_z>(), {data_z{31}, data_z{33}});

    // erase
    components_container container_yz{archetype<data_y, data_z>::type};
    container_xyz.move_entity<data_x>(container_yz, entity3);
    REQUIRE_SET_EQUAL(container_xy.entities(),  {entity2});
    REQUIRE_SET_EQUAL(container_xyz.entities(), {entity1});
    REQUIRE_SET_EQUAL(container_yz.entities(),  {entity3});

    // emplace + erase check
    container_yz.move_entity<data_z>(container_xy, entity3, data_x{31});
    REQUIRE(container_yz.entities().empty());
    REQUIRE_SET_EQUAL(container_xy.entities(),  {entity2, entity3});
    REQUIRE_SET_EQUAL(container_xyz.entities(), {entity1});
}


int main() {
    entity_manager entity_data_manager;
    using Archetype = archetype<data_x, data_y>;
    components_container container{Archetype::type};

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

    test_inter_container_entity_move();

    return 0;
}
