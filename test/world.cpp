#include <tower120/ecs/world.hpp>
#include <tower120/ecs/entity.hpp>
#include <tower120/ecs/query.hpp>
#include "test_utils.hpp"


struct data_x{
    int x;
};
struct data_y{
    int y;
};
struct data_z{
    int z;
};


void test_make_destroy_entity(){
    using namespace tower120::ecs;

    world world;
    entity ent = world.make_entity(data_x{1}, data_y{2});
    world.destroy_entity(ent);
}


void test_containers_query(){
     tower120::ecs::world world;

    world.make_entity(data_x{11}, data_y{21});
    world.make_entity(data_x{12});
    world.make_entity(data_x{13}, data_y{23});

    // access query through ADL

    // test foreach
    {
        std::vector<int> xs, ys;
        query(world).foreach([&](const data_x& x, data_y& y) -> bool /*proceed*/{
            xs.emplace_back(x.x);
            ys.emplace_back(y.y);
            return true;
        });
        REQUIRE_EQUAL(xs, {11,13});
        REQUIRE_EQUAL(ys, {21,23});
    }
    {
        std::vector<int> xs, ys;
        query(world).foreach([&](tower120::ecs::entity, const data_x& x, data_y& y){
            xs.emplace_back(x.x);
            ys.emplace_back(y.y);
        });
        REQUIRE_EQUAL(xs, {11,13});
        REQUIRE_EQUAL(ys, {21,23});
    }

    // test query_result
    {
        world.make_entity(data_x{14}, data_y{24}, data_z{34});

        tower120::ecs::query_result result =
            query(world)
            .contains<data_y>()
            .not_contains(tower120::ecs::component_typeid<data_z>)
            .select<data_x>();

        std::vector<int> xs;
        for(auto&&[ent, d_x] : result){
            (void)ent;
            xs.emplace_back(d_x.x);
        }
        REQUIRE_EQUAL(xs, {11,13});
    }
}


void test_change_components(){
    using namespace tower120::ecs;
    world world;

    entity e1 = world.make_entity(data_x{11}, data_y{21});
    world.change_components<data_x>(e1, data_z{31});

    REQUIRE( e1.archetype() == archetype<data_y, data_z>::type );
    REQUIRE( e1.component<data_y>().y == 21 );
    REQUIRE( e1.component<data_z>().z == 31 );
}


int main() {
    test_make_destroy_entity();
    test_containers_query();
    test_change_components();
}