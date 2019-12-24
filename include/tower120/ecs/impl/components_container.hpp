#pragma once

#include "../entity.hpp"
#include <tower120/ecs/component.hpp>
#include "../archetype.hpp"
#include "utils/algorithm.hpp"
#include "utils/static_any.hpp"
#include "utils/type_constant.hpp"

#include <deque>
#include <vector>
#include <cassert>
#include <utility>
#include <algorithm>


namespace tower120::ecs::impl{


    /*template<template<class...> class Container, class ContainerExample = Container<int>>
    class type_erased_container{
    public:
        type_erased_container(const type_erased_container&) = delete;
        type_erased_container(type_erased_container&&)      = delete;

        template<class T, class ...Args>
        explicit type_erased_container(Args&&...args){
            static_assert(sizeof(ContainerExample) >= sizeof(T));

            new(storage) T(std::forward<Args>(args)...);
            container_destructor = [](void* address){
                static_cast<T*>(address)->~T();
            };
        }

        template<class T>
        T& get(){
            assert(type_id == typeid(T));
            return *static_cast<T*>(storage);
        }

        ~type_erased_container(){
            container_destructor();
        }
    // -----------------------------------
    //           DATA
    // -----------------------------------
    private:
        std::byte storage[sizeof(ContainerExample)];
        void (*container_destructor)(void*);

        #ifndef NDEBUG
        std::type_info type_id;
        #endif
    };*/



    class components_container{
    // -----------------------------------
    //          TYPES
    // -----------------------------------
    private:
        template<class T> using component_list_t = std::vector<T>;
        using type_erased_component_list_t = static_any<sizeof(component_list_t<int>)>;

    // -----------------------------------
    //          INTERFACE
    // -----------------------------------
    public:
        components_container(const components_container&) = delete;
        components_container(components_container&&)      = delete;

        template<class ...Components>
        explicit components_container(entity_manager& entity_manager)
            : entity_manager(entity_manager)
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
                const auto type_id = component_type_id<T>();
                component_types[index] = type_id;
            }, integral_constant_sequence_for<Components...>{});
        }

        template<class Component>
        Component& component(entity entity){
            entity_data& entity_data = entity_manager.data(entity);
            assert(entity_data.components_container != nullptr);
            return components<Component>()[entity_data.container_index];
        }

        template<class Component>
        component_list_t<Component>& components(){
            const auto type_id = component_type_id<Component>();

            // linear search is fastest for <20-50 elements
            const auto iter = std::find(component_types.begin(), component_types.end(), type_id);
            assert(iter != component_types.end());

            const std::size_t index = std::distance(component_types.begin(), iter);
            return components_lists[index].get< component_list_t<Component> >();
        }

    // -----------------------------------
    //          IMPLEMENTATION
    // -----------------------------------
    private:


    // -----------------------------------
    //           DATA
    // -----------------------------------
    private:
        entity_manager& entity_manager;
        std::vector<entity> entities;
        std::vector<type_erased_component_list_t> components_lists;

        std::vector<component_type_id_t> component_types;       // TODO: small vector here
    };

}