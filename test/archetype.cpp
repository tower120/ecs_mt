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
    archetype arch1(std::initializer_list<component_type>{
        component_type_of<data_x>,
        component_type_of<data_y>,
    });
    REQUIRE(arch1.contains(component_type_of<data_x>));
    REQUIRE(arch1.contains(component_type_of<data_y>));
    REQUIRE(!arch1.contains(component_type_of<data_z>));

    archetype arch2(std::initializer_list<component_type>{
        component_type_of<data_x>,
        component_type_of<data_y>,
    });

    REQUIRE(arch1 == arch2);

    archetype arch3(std::initializer_list<component_type>{
        component_type_of<data_x>,
        component_type_of<data_y>,
        component_type_of<data_z>,
    });

    REQUIRE(arch1 != arch3);


    // component_index test
    archetype_t<data_x, data_y> ct_archetype;
    REQUIRE(
        arch1.component_index(component_type_of<data_x>) == ct_archetype.component_index<data_x>()
    );
    REQUIRE(
        arch1.component_index(component_type_of<data_y>) == ct_archetype.component_index<data_y>()
    );

    return 0;
}

