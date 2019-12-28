#include "test_utils.hpp"

#include <tower120/ecs/archetype.hpp>
#include "test_utils.hpp"

int main() {
    using namespace tower120::ecs;

    struct data_x : component<data_x>{
        int x;
    };
    struct data_y : component<data_y>{
        int y;
    };
    struct data_z : component<data_z>{
        int z;
    };


    archetype arch1(std::initializer_list<component_type_t>{
        data_x::component_type,
        data_y::component_type,
    });
    REQUIRE(arch1.contains(data_x::component_type));
    REQUIRE(arch1.contains(data_y::component_type));
    REQUIRE(!arch1.contains(data_z::component_type));

    archetype arch2(std::initializer_list<component_type_t>{
        data_y::component_type,
        data_x::component_type,
    });

    REQUIRE(arch1 == arch2);

    archetype arch3(std::initializer_list<component_type_t>{
        data_y::component_type,
        data_x::component_type,
        data_z::component_type,
    });

    REQUIRE(arch1 != arch3);


    archetype_t<data_x, data_y> ct_archetype;
    REQUIRE(
        arch1.component_index(data_x::component_type) == ct_archetype.component_index<data_x>()
    );
    REQUIRE(
        arch1.component_index(data_y::component_type) == ct_archetype.component_index<data_y>()
    );


    return 0;
}

