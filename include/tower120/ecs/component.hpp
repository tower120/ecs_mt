#pragma once

#include <cstdint>
#include <tower120/ecs/impl/utils/static_any.hpp>
#include <tower120/ecs/impl/components_container_types.hpp>

namespace tower120::ecs{

    /*template<class Component>
    uint16_t component_id() {
        const static uint16_t type_id = impl::utils::monotonic_counter<uint16_t, Component>::get();
        return type_id;
    }*/

    //using component_type_t = uint16_t;

    // for user defined specializations
    /*template<class Component>
    component_type_t get_component_type();

    class component{};*/

    /*template<class T>
    inline constexpr bool is_component = std::is_base_of_v<component, T>;*/


    struct component_type_data{
        using make_type_erased_component_list_t =
            impl::components_container_types::type_erased_component_list_t(*)();
        make_type_erased_component_list_t make_type_erased_component_list;

//        using emplace_entity_t = void(*)(void*);
//        emplace_entity_t emplace_entity;

        // move entity (entity, type_erased_component_list_t from, type_erased_component_list_t to)
        // erase entity (entity, type_erased_component_list_t)
    };

    // TODO: make concrete type wrapper
    using component_type = const component_type_data*;


    // TODO:
    // pointer to static constexpr may work wrong across dll boundaries
    // since each dll have its own set of statics. Initialization order and references may be not the same
    // need more stable way to get component id.
    // Solution:
    //   pass ID as template argument and use it
    template<class Component>
    class component_info{
    private:
        const constexpr static component_type_data type_data{
             /*.make_type_erased_component_list = */ [](){
                 using namespace impl::components_container_types;
                 return type_erased_component_list_t{component_list_t<Component>()};
             }
        };
    public:
         static constexpr const component_type type = &type_data;
    };

    template<class Component>
    constexpr const component_type component_type_of = component_info<Component>::type;

}