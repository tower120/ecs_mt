#include <tower120/ecs/world.hpp>
#include "test_utils.hpp"

int main() {
    using namespace tower120::ecs;

    struct data_x{
        int x;
    };
    struct data_y{
        int y;
    };


    world world;
    world.make_entity(data_x{1}, data_y{2});
}