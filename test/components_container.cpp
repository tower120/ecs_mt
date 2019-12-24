#include "test_utils.hpp"

#include <tower120/ecs/impl/components_container.hpp>
#include "test_utils.hpp"

int main() {
    using namespace tower120::ecs;
    //using namespace ranges;

    struct data_x{
        int x;
    };
    struct data_y{
        int y;
    };

    entity_manager entity_data_manager;
    impl::components_container container(entity_manager);

    // test emplace
    /*auto obj_ref = container.emplace();
    const entity first_entity = obj_ref.entity;
    std::get<data_x&>(obj_ref.components).x = 11;
    obj_ref.component<data_y>().y = 12;

    // test iterators
    REQUIRE((*container.begin()).component<data_x>().x == 11);
    REQUIRE((*container.begin()).component<data_y>().y == 12);

    // test range
    container.emplace(data_x{21}, data_y{22});
    container.emplace(data_x{31}, data_y{32});

    const auto show_container = [&](){
        for(auto obj : container){
            std::cout
                << obj.component<data_x>().x
                << ":"
                << obj.component<data_y>().y
                << std::endl;
        }
    };
    show_container();

    // test erase
    std::cout << "test erase" << std::endl;
    container.erase(first_entity);
    show_container();*/

    /*REQUIRE_EQUAL(views::transform(container, &impl::archetype_object_ref<data_x, data_y>::components), {
        components_tuple{{31}, {32}},
        components_tuple{{21}, {22}}
    });*/

    return 0;
}
