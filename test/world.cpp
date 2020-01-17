#include <tower120/ecs/world.hpp>
#include "test_utils.hpp"

using namespace tower120::ecs;

struct data_x{
    int x;
};
struct data_y{
    int y;
};


void test_make_destroy_entity(){
    world world;
    entity ent = world.make_entity(data_x{1}, data_y{2});
    world.destroy_entity(ent);
}


void test_containers_query(){
    world world;

    // test query
    world.make_entity(data_x{11}, data_y{21});
    world.make_entity(data_x{12});
    world.make_entity(data_x{13}, data_y{23});

    /*world.query_foreach_container(
        std::array{component_typeid<data_x>, component_typeid<data_y>},
        std::array<component_typeinfo, 0>{},
        [](impl::components_container& container) -> bool {
            for(data_x& x : container.components<data_x>()){
                std::cout << x.x  << ",";
            }
            return true;
        }
    );*/


    // TODO: test foreach()


    auto result = world.query<data_x>().contains<data_y>().select();
    /*auto result = world.query<data_x>().contains<data_y>().select();
    auto result = world.query<data_x>().contains<data_y>().foreach([](){});

    query_result = query_t(world).select<data_x, data_y>();
    query_result = query_t(world).contains<data_y>().select<data_x, data_y>();

    query_t(world).foreach([](data_x&, data_y&){});
    query_t(world).foreach_container();*/


    /*auto first = *result.begin();
    auto sent  = result.end();

    (void)first;
    (void)sent;*/

    //for(auto[ent, d_x] : result){
    for(auto pair : result){
        (void)pair;
        std::cout << "";
        //d_x.x += 2;
    }

}


int main() {
    test_make_destroy_entity();
    test_containers_query();
}