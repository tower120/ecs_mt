#pragma once

#include "../entity.hpp"
#include <tower120/ecs/component.hpp>
#include "../archetype.hpp"
#include "utils/algorithm.hpp"
#include "utils/static_any.hpp"
#include "utils/type_constant.hpp"

#include "components_container_types.hpp"

#include <deque>
#include <vector>
#include <cassert>
#include <utility>
#include <algorithm>

namespace tower120::ecs::impl{

    //
    // component_list ordered by component_type_t
    //
    class components_container{
    // -----------------------------------
    //          TYPES
    // -----------------------------------
    private:
        //template<class T> using component_list_t = std::vector<T>;
        //using type_erased_component_list_t = static_any<sizeof(component_list_t<int>)>;

        template<class T> using component_list_t = components_container_types::component_list_t<T>;
        using type_erased_component_list_t = components_container_types::type_erased_component_list_t;

    // -----------------------------------
    //          INTERFACE
    // -----------------------------------
    public:
        components_container(const components_container&) = delete;
        components_container(components_container&&)      = delete;

        /*template<class ...Components>
        explicit components_container(entity_manager& entity_manager)
            : entity_manager(entity_manager)
            , archetype(archetype<Components...>{})
        {
            using namespace utils;

            using components_tuple = std::tuple<Components...>;

            // fill components
            components_lists.resize(sizeof...(Components));
            foreach([&](auto integral_constant){
                constexpr const std::size_t index = integral_constant.value;
                using T = std::tuple_element_t<index, components_tuple>;
                components_lists[index].template emplace<component_list_t<T>>();
            }, integral_constant_sequence_for<Components...>{});

            // fill component_type_to_index
            component_types.resize(sizeof...(Components));
            foreach([&](auto integral_constant){
                constexpr const std::size_t index = integral_constant.value;
                using T = std::tuple_element_t<index, components_tuple>;
                const auto type_id = get_component_type<T>();
                component_types[index] = type_id;
            }, integral_constant_sequence_for<Components...>{});
        }*/

        components_container(entity_manager& entity_manager, const archetype& archetype)
            : entity_manager(entity_manager)
            , archetype(archetype)
        {
            const auto& components = archetype.components();
            const std::size_t size = components.size();
            components_lists.reserve(size);
            for(component_type_t component_type : components){
                components_lists.emplace_back(
                    component_type->make_type_erased_component_list()
                );
            }
        }

        template<class Component>
        Component& component(entity entity){
            entity_data& entity_data = entity_manager.data(entity);
            assert(entity_data.components_container != nullptr);
            return components<Component>()[entity_data.container_index];
        }

        template<class Component, class Archetype>
        Component& component(entity entity){
            entity_data& entity_data = entity_manager.data(entity);
            assert(entity_data.components_container != nullptr);
            return components<Component, Archetype>()[entity_data.container_index];
        }

        template<class Component>
        component_list_t<Component>& components(){
            const std::size_t index = archetype.component_index(Component::component_type);
            return components_lists[index].get< component_list_t<Component> >();
        }

        template<class Component, class Archetype>
        component_list_t<Component>& components(){
            const std::size_t index = Archetype::template component_index<Component>();
            return components_lists[index].get< component_list_t<Component> >();
        }


        // all components must be default constructible
//        void emplace(entity entity){
//            entities.emplace_back(entity);
//            for(const type_erased_component_list_t& components_list : components_lists){
//                components_list.get<>
//            }
//        }

        // TODO : piecewise construct?
        template<class ...Components>
        void emplace(entity entity, Components...components){
            using Archetype = archetype_t<Components...>;
            assert(Archetype::archetype == this->archetype);

            entities.emplace_back(entity);
            (this->components<Components, Archetype>().emplace_back( std::move(components) ), ...);
        }

        /*template<class ...Components>
        archetype_object_ref<Components...> emplace();*/

    // -----------------------------------
    //          IMPLEMENTATION
    // -----------------------------------
    private:


    // -----------------------------------
    //           DATA
    // -----------------------------------
    private:
        // const
        entity_manager& entity_manager;     // TODO: remove
        const archetype archetype;

        std::vector<entity> entities;
        std::vector<type_erased_component_list_t> components_lists;
    };

} // namespace