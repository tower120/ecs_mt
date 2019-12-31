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

    // -----------------------------------
    //          INTERFACE
    // -----------------------------------
    public:
        components_container(const components_container&) = delete;
        components_container(components_container&&)      = delete;

        components_container(entity_manager& entity_manager, const archetype& archetype)
            : entity_manager(entity_manager)
            , archetype(archetype)
        {
            const auto& component_types = archetype.components();
            const std::size_t size = component_types.size();

            components_arrays.reserve(size);
            for(component_type component_type : component_types){
                components_arrays.emplace_back( component_type->make_any_vector() );
            }
        }

        template<class Component>
        Component& component(entity entity){
            const entity_data& entity_data = entity_manager.data(entity);
            assert(is_valid_entity(entity));
            return components<Component>()[entity_data.container_index];
        }

        template<class Component, class Archetype>
        Component& component(entity entity){
            const entity_data& entity_data = entity_manager.data(entity);
            assert(is_valid_entity(entity));
            assert(Archetype::archetype == this->archetype);
            return components<Component, Archetype>()[entity_data.container_index];
        }

        template<class Component>
        std::vector<Component>& components(){
            const std::size_t index = archetype.component_index(Component::component_type);
            return components_arrays[index].cast< std::vector<Component> >();
        }

        template<class Component, class Archetype>
        std::vector<Component>& components(){
            assert(Archetype::archetype == this->archetype);
            const std::size_t index = Archetype::template component_index<Component>();
            return components_arrays[index].cast< std::vector<Component> >();
        }


        // all components must be default constructible
//        void emplace(entity entity){
//            entities.emplace_back(entity);
//            for(const type_erased_component_list_t& components_list : components_lists){
//                components_list.get<>
//            }
//        }

        template<class ...Components>
        void emplace(entity entity, Components...components){
            using Archetype = archetype_t<Components...>;
            assert(Archetype::archetype == this->archetype);

            assert(entity_manager.data(entity).components_container == nullptr);

            entities.emplace_back(entity);
            (this->components<Components, Archetype>().emplace_back( std::move(components) ), ...);
            assert(is_valid_components_matrix());
        }

    // -----------------------------------
    //          IMPLEMENTATION
    // -----------------------------------
    private:
        [[nodiscard]]
        bool is_valid_entity(entity entity) const {
            const entity_data& entity_data = entity_manager.data(entity);
                if (entity_data.components_container != this) return false;
            const std::size_t index = entity_data.container_index;
                if (index >= entities.size()) return false;
                if (entities[index] != entity) return false;
            return true;
        }

        [[nodiscard]]
        bool is_valid_components_matrix(){
            std::size_t size = entities.size();
            for(const auto& components_array :  components_arrays){
                if (size != components_array.size()) return false;
            }
            return true;
        }

    // -----------------------------------
    //           DATA
    // -----------------------------------
    private:
        // const
        entity_manager& entity_manager;     // TODO: remove
        const archetype archetype;

        std::vector<entity> entities;
        std::vector<any_vector> components_arrays;
    };

} // namespace