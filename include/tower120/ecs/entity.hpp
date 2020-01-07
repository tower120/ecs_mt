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


    struct entity_data{
        entity_data(const entity_data&) = delete;
        entity_data(entity_data&&) = delete;
        explicit entity_data(std::uint32_t entity_index)
            : entity_index(entity_index) {}

        impl::components_container* components_container = nullptr;
        std::uint32_t container_index;
        std::uint32_t entity_index;
    };


    class entity{
        friend entity_manager;
        friend impl::components_container;
        explicit entity(entity_data& data) noexcept : data(&data){}
    public:
        bool operator==(const entity& other) const noexcept {
            return data == other.data;
        }
        bool operator!=(const entity& other) const noexcept {
            return !(*this == other);
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