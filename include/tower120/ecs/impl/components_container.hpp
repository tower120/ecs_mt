#pragma once

#include "../entity.hpp"
#include <tower120/ecs/component.hpp>
#include "../archetype.hpp"
#include "utils/algorithm.hpp"
#include "utils/type_constant.hpp"
#include "utils/numeric_cast.hpp"

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

        components_container(const archetype_typeinfo& archetype)
            : archetype(archetype)
        {
            const auto& component_types = archetype.components();

            components_arrays.reserve(component_types.size());
            for(component_typeinfo component_type : component_types){
                components_arrays.emplace_back( component_type->make_any_vector() );
            }
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return entities.size();
        }

        template<class Component>
        [[nodiscard]]
        Component& component(entity entity){
            const entity_data& entity_data = *entity.data;
            assert(is_valid_entity(entity));
            return components<Component>()[entity_data.container_index];
        }

        template<class Component, class Archetype>
        [[nodiscard]]
        Component& component(entity entity){
            const entity_data& entity_data = *entity.data;
            assert(is_valid_entity(entity));
            assert(Archetype::typeinfo == this->archetype);
            return components<Component, Archetype>()[entity_data.container_index];
        }

        template<class Component>
        [[nodiscard]]
        std::vector<Component>& components(){
            const std::size_t index = archetype.component_index(component_typeid<Component>);
            return components_arrays[index].cast< std::vector<Component> >();
        }

        template<class Component, class Archetype>
        [[nodiscard]]
        std::vector<Component>& components(){
            assert(Archetype::typeinfo == this->archetype);
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
        void emplace(entity entity, Components...components_){
            entity_data& entity_data = *entity.data;
            using Archetype = tower120::ecs::archetype<Components...>;
            assert(Archetype::typeinfo == this->archetype);
            assert(entity_data.components_container == nullptr);

            entities.emplace_back(entity);
            (this->components<Components, Archetype>().emplace_back( std::move(components_) ), ...);

            assert(is_valid_components_matrix());

            // update entity
            entity_data.components_container = this;
            entity_data.container_index = impl::utils::numeric_cast<std::uint32_t>(entities.size() - 1);
        }

        void erase(entity entity){
            assert(is_valid_entity(entity));
            entity_data& entity_data = *entity.data;

            using namespace impl::utils;

            // do erase
            const auto index = entity_data.container_index;
            unordered_erase(entities, entities.begin() + index);
            for(any_vector& component_array : components_arrays){
                component_array.unordered_erase(index);
            }
            assert(is_valid_components_matrix());

            // update entity
            entity_data.components_container = nullptr;
        }

        template<class Archetype>
        void erase(entity entity){
            assert(is_valid_entity(entity));
            using namespace impl::utils;

            entity_data* entity_data = entity.data;

            // do erase
            const auto index = entity_data->container_index;
            unordered_erase(entities, entities.begin() + index);
            foreach<typename Archetype::components>([&](auto type_constant){
                using Component  = typename decltype(type_constant)::type;
                auto& components = this->components<Component, Archetype>();
                unordered_erase(components, components.begin() + index );
            });
            assert(is_valid_components_matrix());

            // update entity
            entity_data->components_container = nullptr;
        }

    // -----------------------------------
    //          IMPLEMENTATION
    // -----------------------------------
    private:
        [[nodiscard]]
        bool is_valid_entity(entity entity) const {
            const entity_data& entity_data = *entity.data;
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
    public:
        const archetype_typeinfo archetype;
    private:
        std::vector<entity> entities;
        std::vector<any_vector> components_arrays;
    };

} // namespace