#pragma once

#include "../entity.hpp"
#include "../archetype.hpp"
#include "utils.hpp"

#include <deque>
#include <vector>
#include <cassert>
#include <algorithm>

namespace tower120::ecs::impl{

    template<class ...Components>
    struct archetype_object_ref {
        const entity entity;
        std::tuple<Components&...> components;

        template<class Component>
        Component& component() noexcept { return std::get<Component&>(components); }
    };

    template<class ...Components>
    class archetype_container{
    // -----------------------------------
    //          TYPES
    // -----------------------------------
    private:
        template<class T> using component_list = std::vector<T>;    // TODO: replace with fixed size and chunks?
    // -----------------------------------
    //          INTERFACE
    // -----------------------------------
    public:
        archetype_container(const archetype_container&) = delete;
        archetype_container(archetype_container&&)      = delete;

        explicit archetype_container(entity_data_manager& entity_data_manager) noexcept
            : entity_data_manager(entity_data_manager)
        {}


        archetype_object_ref<Components...> emplace(){
            // make entity
            const entity entity = emplace_entity();

            // make components
            std::apply([](auto& ...component_list){
                (component_list.emplace_back(), ...);
            }, components);

            check_components_matrix();

            return {
                entity,
                {std::get<component_list<Components>>(components).back()...}
            };
        }

        archetype_object_ref<Components...> emplace(Components... ctr_components){
            // make entity
            const entity entity = emplace_entity();

            // make components
            foreach([&](auto& ctr_component){
                using component_t = std::decay_t<decltype(ctr_component)>;
                std::get<component_list<component_t>>(components).emplace_back(std::move(ctr_component));
            }, ctr_components...);

            check_components_matrix();

            return {
                entity,
                {std::get<component_list<Components>>(components).back()...}
            };
        }

        void erase(entity entity){
            assert(entity.data->archetype_container == this);
            const std::size_t index = entity.data->index;

            // erase from entities
            unordered_erase(entities, entities.begin() + index);
            // update index
            if (entities.size() > index)
                entities[index].data->index = index;

            // erase from components
            foreach([&](auto& component_list){
                unordered_erase(component_list, component_list.begin() + index);
            }, components);

            entity_data_manager.free(*entity.data);
        }

        // iterator mainly for debug purposes now.

        template<bool is_const>
        class iterator_t{
            friend archetype_container<Components...>;

            std::conditional_t<is_const
                , std::vector<entity>::const_iterator
                , std::vector<entity>::iterator
            > entity_iterator;

            template<class T> using component_iterator =
                std::conditional_t<is_const
                    , typename component_list<T>::const_iterator
                    , typename component_list<T>::iterator
                >;
            std::tuple< component_iterator<Components>... > components_iterator;

            iterator_t(decltype(entity_iterator) entity_iterator, decltype(components_iterator) components_iterator)
                : entity_iterator(entity_iterator)
                , components_iterator(components_iterator)
            {}
        public:
            iterator_t() = default;

            std::conditional_t <is_const
                , archetype_object_ref<const Components...>
                , archetype_object_ref<Components...>
            > operator*() const {
                return {
                    *entity_iterator,
                     {*std::get<component_iterator<Components>>(components_iterator)...}
                };
            }

            iterator_t& operator++(){
                ++entity_iterator;
                std::apply([](auto& ...component_iterator){
                    (++component_iterator, ...);
                }, components_iterator);
                return *this;
            }

            iterator_t operator++(int) const {
                iterator_t iter{*this};
                ++iter;
                return iter;
            }

            bool operator==(const iterator_t& other) const noexcept {
                return entity_iterator == other.entity_iterator;
            }
            bool operator!=(const iterator_t& other) const noexcept {
                return !(*this == other);
            }
        };

        using iterator       = iterator_t<false>;
        using const_iterator = iterator_t<true>;

        iterator begin() {
            return {
                entities.begin(),
                {std::get<component_list<Components>>(components).begin()...}
            };
        }
        iterator end() {
            return {
                entities.end(),
                {std::get<component_list<Components>>(components).end()...}
            };
        }

        const_iterator begin() const {
            return {
                entities.begin(),
                {std::get<component_list<Components>>(components).begin()...}
            };
        }
        const_iterator end() const{
            return {
                entities.end(),
                {std::get<component_list<Components>>(components).end()...}
            };
        }


    // -----------------------------------
    //          IMPLEMENTATION
    // -----------------------------------
    private:
        void check_components_matrix(){
            const std::array<std::size_t, sizeof...(Components)> sizes =
                {std::get<component_list<Components>>(components).size()...};

            const bool all_equal = std::adjacent_find( sizes.begin(), sizes.end(), std::not_equal_to<>() ) == sizes.end();

            assert(all_equal);
        }

        entity emplace_entity(){
            entity_data& entity_data = entity_data_manager.make();
            entity entity{entity_data};
            entities.push_back(entity);

            entity_data.archetype_container = this;
            entity_data.index = entities.size() - 1;

            return entity;
        }

    // -----------------------------------
    //           DATA
    // -----------------------------------
    private:
        entity_data_manager& entity_data_manager;
        std::vector<entity> entities;
        std::tuple< component_list<Components>... > components;
    };

}