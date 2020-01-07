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
    archetype_typeinfo arch1(std::initializer_list<component_typeinfo>{
            component_typeid<data_x>,
            component_typeid<data_y>,
    });
    REQUIRE(arch1.contains(component_typeid<data_x>));
    REQUIRE(arch1.contains(component_typeid<data_y>));
    REQUIRE(!arch1.contains(component_typeid<data_z>));

    archetype_typeinfo arch2(std::initializer_list<component_typeinfo>{
            component_typeid<data_x>,
            component_typeid<data_y>,
    });

    REQUIRE(arch1 == arch2);

    archetype_typeinfo arch3(std::initializer_list<component_typeinfo>{
            component_typeid<data_x>,
            component_typeid<data_y>,
            component_typeid<data_z>,
    });

    REQUIRE(arch1 != arch3);


    // component_index test
    archetype<data_x, data_y> ct_archetype;
    REQUIRE(
            arch1.component_index(component_typeid<data_x>) == ct_archetype.component_index<data_x>()
    );
    REQUIRE(
            arch1.component_index(component_typeid<data_y>) == ct_archetype.component_index<data_y>()
    );

    return 0;
}

