#pragma once

#include <cstdio>
#include <deque>
#include <vector>

namespace tower120::ecs{

    namespace impl{
        template<class...> class archetype_container;
    }


    // entity map
    // unordered_map<entity, std::pair<archetype_container*, index>>


    struct entity_data{
        entity_data(const entity_data&) = delete;
        entity_data(entity_data&&) = delete;
        entity_data() = default;

        void* archetype_container = nullptr;
        std::size_t index;
    };


    class entity_data_manager{
    public:
        entity_data_manager(const entity_data_manager&) = delete;
        entity_data_manager(entity_data_manager&&)      = delete;
        entity_data_manager() = default;

        entity_data& make(){
            if (!free_list.empty()){
                entity_data& ed = *free_list.back();
                free_list.pop_back();
                return ed;
            }

            return list.emplace_back();
        }

        void free(entity_data& ed){
            ed.archetype_container = nullptr;
            free_list.emplace_back(&ed);
        }

    private:
        std::deque<entity_data> list;
        std::vector<entity_data*> free_list;
    };


    class entity{
        template<class...> friend class impl::archetype_container;
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

}