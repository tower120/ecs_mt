#pragma once

#include "impl/utils/numeric_cast.hpp"

#include <cstdio>
#include <deque>
#include <vector>
#include <cstdint>
#include <cassert>

namespace tower120::ecs{

    namespace impl{
        class components_container;
    }
    class entity_manager;
    class world;
    class archetype_typeinfo;


    struct entity_data{
        entity_data(const entity_data&) = delete;
        entity_data(entity_data&&) = delete;
        explicit entity_data(std::uint32_t entity_index)
            : entity_index(entity_index) {}

        impl::components_container* components_container = nullptr;
        std::uint32_t container_index;      // index in components_container
        std::uint32_t entity_index;         // index in entity_manager
    };


    class entity{
        friend entity_manager;
        friend impl::components_container;
        friend world;
        constexpr explicit entity(entity_data& data) noexcept : data(&data){}
    public:
        [[nodiscard]]
        constexpr bool operator==(const entity& other) const noexcept {
            return data == other.data;
        }

        [[nodiscard]]
        constexpr bool operator!=(const entity& other) const noexcept {
            return !(*this == other);
        }

        [[nodiscard]]
        constexpr explicit operator bool() const noexcept{
            return data->components_container != nullptr;
        }

        [[nodiscard]]
        const archetype_typeinfo& archetype() const noexcept{
            return get_archetype(this);
        }

        template<class Component>
        [[nodiscard]]
        Component& component(){
            return get_component<Component>(this);
        }
    private:
        template<class Self>
        static const archetype_typeinfo& get_archetype(const Self* self) noexcept {
            return self->data->components_container->archetype;
        }
        template<class Component, class Self>
        static decltype(auto) get_component(Self* self){
            return self->data->components_container->template component<Component>(*self);
        }

    private:
        entity_data* data;  // not null
    };


    class entity_manager{
    public:
        entity_manager(const entity_manager&) = delete;
        entity_manager(entity_manager&&)      = delete;
        entity_manager() = default;

        entity make(){
            if (!free_list.empty()){
                entity_data* data = free_list.back();
                free_list.pop_back();
                return entity{*data};
            }
            const std::uint32_t index = impl::utils::numeric_cast<std::uint32_t>(list.size());
            return entity{list.emplace_back(index)};
        }

        void free(entity entity){
            assert(entity.data->components_container == nullptr);
            free_list.emplace_back(entity.data);
        }

    private:
        std::deque<entity_data> list;
        std::vector<entity_data*> free_list;
    };

}