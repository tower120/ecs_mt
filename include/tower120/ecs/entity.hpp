#pragma once

#include <cstdio>
#include <deque>
#include <vector>

namespace tower120::ecs{

    namespace impl{
        class components_container;
    }
    class entity_manager;


    struct entity_data{
        /*entity_data(const entity_data&) = delete;
        entity_data(entity_data&&) = delete;
        entity_data() = default;*/

        impl::components_container* components_container = nullptr;
        std::size_t container_index;
    };


    // Alternatively (probably should be the same performance-wise):
    // entity can hold direct pointer to entity_data
    // entity_data needs to store entity_data_index
    // entity_manager::list needs to be deque
    // See archetype_container branch
    class entity{
        friend entity_manager;
    public:
        using index_type = uint32_t;
    private:
        explicit entity(entity::index_type entity_data_index) noexcept : entity_data_index(entity_data_index){}
    public:
        bool operator==(const entity& other) const noexcept {
            return entity_data_index == other.entity_data_index;
        }
        bool operator!=(const entity& other) const noexcept {
            return !(*this == other);
        }
    private:
        index_type entity_data_index;
    };


    class entity_manager{
    public:
        entity_manager(const entity_manager&) = delete;
        entity_manager(entity_manager&&)      = delete;
        entity_manager() = default;

        entity make(){
            if (!free_list.empty()){
                const entity::index_type index = free_list.back();
                free_list.pop_back();
                return entity{index};
            }

            list.emplace_back();
            return entity{(entity::index_type)list.size() - 1};
        }

        entity_data& data(entity entity){
            return list[entity.entity_data_index];
        }

        void free(entity entity){
            entity_data& ed = data(entity);

            ed.components_container = nullptr;
            free_list.emplace_back(ed.container_index);
        }

    private:
        std::vector<entity_data> list;
        std::vector<entity::index_type> free_list;
    };

}