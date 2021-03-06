#include <tower120/ecs/archetype.hpp>
#include "test_utils.hpp"

int main() {
    using namespace tower120::ecs;

    struct data_x {
        int x;
    };
    struct data_y {
        int y;
    };
    struct data_z {
        int z;
    };


    // general test
    archetype_typeinfo arch1{
        component_typeid<data_x>,
        component_typeid<data_y>,
    };
    REQUIRE(arch1.contains(component_typeid<data_x>));
    REQUIRE(arch1.contains(component_typeid<data_y>));
    REQUIRE(!arch1.contains(component_typeid<data_z>));

    archetype_typeinfo arch2{
        component_typeid<data_x>,
        component_typeid<data_y>,
    };

    REQUIRE(arch1 == arch2);

    archetype_typeinfo arch3{
        component_typeid<data_x>,
        component_typeid<data_y>,
        component_typeid<data_z>,
    };

    REQUIRE(arch1 != arch3);


    // component_index test
    archetype<data_x, data_y> ct_archetype;
    REQUIRE(
        arch1.component_index(component_typeid<data_x>) == ct_archetype.component_index<data_x>()
    );
    REQUIRE(
        arch1.component_index(component_typeid<data_y>) == ct_archetype.component_index<data_y>()
    );

    // archetype arithmetic
    // +
    {
        archetype_typeinfo xy  = archetype<data_x, data_y>::type;
        archetype_typeinfo xyz = xy + archetype<data_z>::type;
        REQUIRE(xyz == archetype<data_x, data_y, data_z>::type);
        REQUIRE(xyz + archetype<>::type == xyz);
    }
    // + w duplicates
    {
        archetype_typeinfo xy  = archetype<data_x, data_y>{};
        archetype_typeinfo xyz = xy + archetype<data_y, data_z>{};
        REQUIRE(xyz == archetype<data_x, data_y, data_z>{});
    }
    // -
    {
        archetype_typeinfo xyz = archetype<data_x, data_y, data_z>::type;
        archetype_typeinfo xy  = xyz - archetype<data_z>::type;
        REQUIRE(xy == archetype<data_x, data_y>::type);
        REQUIRE(xyz - archetype<>::type == xyz);
    }

    return 0;
}

