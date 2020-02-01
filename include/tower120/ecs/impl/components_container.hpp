#pragma once

#include "../entity.hpp"
#include <tower120/ecs/component.hpp>
#include "../archetype.hpp"
#include "utils/algorithm.hpp"
#include "utils/output_iterator.hpp"
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

        explicit components_container(const archetype_typeinfo& archetype)
            : archetype(archetype)
        {
            const auto& component_types = archetype.components();

            m_components_arrays.reserve(component_types.size());
            for(component_typeinfo component_type : component_types){
                m_components_arrays.emplace_back( component_type.type_data().make_any_vector() );
            }
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return m_entities.size();
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return size() == 0;
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
            assert(Archetype::type == this->archetype);
            return components<Component, Archetype>()[entity_data.container_index];
        }

        [[nodiscard]]
        const std::vector<entity>& entities() const {
            return m_entities;
        }

        template<class Component>
        [[nodiscard]]
        std::vector<Component>& components(){
            const std::size_t index = archetype.component_index(component_typeid<Component>);
            return m_components_arrays[index].cast< std::vector<Component> >();
        }

        template<class Component, class Archetype>
        [[nodiscard]]
        std::vector<Component>& components(){
            assert(Archetype::type == this->archetype);
            const std::size_t index = Archetype::template component_index<Component>();
            return m_components_arrays[index].cast< std::vector<Component> >();
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
            assert(Archetype::type == this->archetype);
            assert(entity_data.components_container == nullptr);
            m_entities.emplace_back(entity);
            (this->components<Components, Archetype>().emplace_back( std::move(components_) ), ...);

            assert(is_valid_components_matrix());

            // update entity
            entity_data.components_container = this;
            entity_data.container_index = impl::utils::numeric_cast<std::uint32_t>(m_entities.size() - 1);
        }

        void erase(entity entity){
            assert(is_valid_entity(entity));
            entity_data& entity_data = *entity.data;

            using namespace impl::utils;

            // do erase
            const auto index = entity_data.container_index;
            unordered_erase(m_entities, m_entities.begin() + index);
            for(any_vector& component_array : m_components_arrays){
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
            unordered_erase(m_entities, m_entities.begin() + index);
            foreach_tuple<typename Archetype::components>([&](auto type_constant){
                using Component  = typename decltype(type_constant)::type;
                auto& components = this->components<Component, Archetype>();
                unordered_erase(components, components.begin() + index );
            });
            assert(is_valid_components_matrix());

            // update entity
            entity_data->components_container = nullptr;
        }

        template<class ...RemoveComponents, class ...AddComponents>
        void move_entity(components_container& other, entity entity, AddComponents... add_components){
            assert(is_valid_entity(entity));
            assert(archetype
                + tower120::ecs::archetype<AddComponents...>::type
                - tower120::ecs::archetype<RemoveComponents...>::type
                == other.archetype);
            using namespace impl::utils;
            entity_data* entity_data = entity.data;
            const auto element_index = entity_data->container_index;        // entity.data-> cause ICE in GCC

            // 1. Move whatever we can
            const auto container_move_entity = [&](auto&& iter_pair){
                const std::size_t index1 = numeric_cast<std::size_t>(std::distance(this->archetype.components().begin(), iter_pair.first));
                const std::size_t index2 = numeric_cast<std::size_t>(std::distance(other.archetype.components().begin(), iter_pair.second));

                any_vector& components1 = this->m_components_arrays[index1];
                any_vector& components2 = other.m_components_arrays[index2];
                components1.unordered_move_back(components2, element_index);
            };
            set_to_set_map(
                archetype.components(),
                other.archetype.components(),
                output_iterator(std::move(container_move_entity))
            );
            // unordered_move entity to other
            other.m_entities.push_back(std::move(m_entities[element_index]));
            if (element_index != size()){
                m_entities[element_index] = std::move(m_entities.back());
                m_entities[element_index].data->container_index = element_index;   // Update moved (previously last) entity
            }
            m_entities.pop_back();

            // 2. Remove
            set_to_set_map(
                archetype.components(),
                tower120::ecs::archetype<RemoveComponents...>::type.components(),
                output_iterator([&](auto&& iter_pair){
                    const std::size_t index1 = numeric_cast<std::size_t>(std::distance(this->archetype.components().begin(), iter_pair.first));
                    any_vector& components1 = this->m_components_arrays[index1];
                    components1.unordered_erase(element_index);
                })
            );

            const std::size_t other_new_size = other.size();

            // 3. Add non-existent components
            const auto add_components_indexes = other.archetype.components_indexes<AddComponents...>();
            static_for<sizeof...(AddComponents)>([&](auto integral_constant){
                constexpr const auto index = integral_constant.value;
                using Component      = std::tuple_element_t<index, std::tuple<AddComponents...>>;
                Component& component = std::get<index>( std::tie(add_components...) );

                const auto component_array_index = add_components_indexes[index];
                std::vector<Component>& component_array =
                    other.m_components_arrays[component_array_index]
                    .template cast<std::vector<Component>>();

                // play safe. If we already contains that component - just update it.
                if (component_array.size() == other_new_size) /*[[unlikely]]*/ {
                    component_array[other_new_size - 1] = std::move(component);
                } else {
                    component_array.push_back( std::move(component) );
                }
            });

            // Sanity checks
            assert(is_valid_components_matrix());
            assert(other.is_valid_components_matrix());

            // update entity
            entity_data->components_container = &other;
            entity_data->container_index = numeric_cast<std::uint32_t>(other_new_size - 1);
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
                if (index >= m_entities.size()) return false;
                if (m_entities[index] != entity) return false;
            return true;
        }

        [[nodiscard]]
        bool is_valid_components_matrix(){
            const std::size_t size = m_entities.size();
            for(const auto& components_array :  m_components_arrays){
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
        std::vector<entity> m_entities;
        std::vector<any_vector> m_components_arrays;
    };

} // namespace