#pragma once

#include "entity.hpp"
#include "impl/components_container.hpp"
#include <tower120/ecs/impl/utils/subrange.hpp>

#include <unordered_map>

namespace tower120::ecs{

    class query_t;

    class world {
        friend query_t;
    // -----------------------------------
    //            TYPES
    // -----------------------------------
        using components_container = impl::components_container;
    // -----------------------------------
    //          INTERFACE
    // -----------------------------------
    public:
        world() = default;
        world(const world&) = delete;
        world(world&&)      = delete;

        template<class ...Components>
        entity make_entity(Components... components){
            entity entity = entity_manager.make();

            // search appropriate archetype
            using Archetype = archetype<Components...>;
            components_container& container = get_archetype(Archetype::type);

            // emplace
            container.emplace(entity, std::move(components)...);

            return entity;
        }


        void destroy_entity(entity ent){
            assert(ent.data->components_container);
            impl::components_container& components_container = *ent.data->components_container;
            components_container.erase(ent);

            if (components_container.empty())
                destroy_archetype_container(components_container.archetype);
        }


        // Duplicated add_component will be overwritten with new one
        template<class ...RemoveComponents, class ...AddComponents>
        void change_components(entity ent, AddComponents... add_components){
            components_container& old_container = *ent.data->components_container;

            const archetype_typeinfo result_archetype =
                old_container.archetype
                    - archetype<RemoveComponents...>::type
                    + archetype<AddComponents...>::type;

            components_container& new_container = get_archetype(result_archetype);
            old_container.move_entity<RemoveComponents...>(new_container, ent, std::move(add_components)...);

            if (old_container.empty())
                destroy_archetype_container(old_container.archetype);
        }

    // -----------------------------------
    //          IMPLEMENTATION
    // -----------------------------------
    private:
        components_container& get_archetype(const archetype_typeinfo& archetype){
            const auto found = archetypes.find(archetype);
            if (found == archetypes.end()){
                return make_archetype_container(archetype);
            } else {
                return found->second;
            }
        }

        components_container& make_archetype_container(const archetype_typeinfo& archetype){
            // construct container per se
            auto pair = archetypes.emplace(archetype, archetype);   assert(pair.second);
            components_container& components_container = pair.first->second;

            // fill components matrix
            for(component_typeinfo component_type : archetype.components()){
                components.emplace(component_type, &components_container);
            }
            assert(is_valid_components());

            return components_container;
        }


        void destroy_archetype_container(const archetype_typeinfo& archetype){
            using namespace impl::utils;
            for(component_typeinfo component_type : archetype.components()){
                // erase only with specific archetype
                subrange found{components.equal_range(component_type)};
                assert(!found.empty());
                for (auto i = found.begin(), last = found.end(); i != last; ++i) {
                    const components_container& container = *i->second;
                    if (container.archetype.hash() == archetype.hash()
                        && container.archetype == archetype)
                    {
                        components.erase(i);
                        break;
                    }
                }
            }

            assert(archetypes.count(archetype) == 1);
            archetypes.erase(archetype);

            assert(is_valid_components());
        }


        bool is_valid_components() const {
            // count total components count
            std::size_t total_components = 0;
            for(const auto& key_value : archetypes){
                const archetype_typeinfo& arch_type = key_value.first;
                total_components += arch_type.components().size();

                // each archetype must have exactly one component
                for(component_typeinfo component : arch_type.components()){
                    std::size_t occurrences = 0;
                    const auto range = components.equal_range(component);
                    for (auto it = range.first; it != range.second; ++it) {
                        if (it->second->archetype == arch_type)
                            occurrences++;
                    }

                    if (occurrences != 1) return false;
                }
            }

            if (total_components != components.size()) return false;
            return true;
        }


        template<class ContainsRange, class NotContainsRange, class Closure>
        void query_foreach_container(
            const ContainsRange& contains_range,
            const NotContainsRange& not_contains_range,
            Closure&& closure
        ){
            using namespace impl::utils;

            const auto check_container = [&](const components_container& container) -> bool{
                // check "contains"
                for(component_typeinfo component_type : contains_range){
                    if (!container.archetype.contains(component_type)){
                        return false;
                    }
                }

                // check "not contains"
                for(component_typeinfo component_type : not_contains_range){
                    if (container.archetype.contains(component_type)){
                        return false;
                    }
                }

                return true;
            };

            // 1. Find component_typeinfo with minimal occurrence
            subrange<decltype(components)::iterator> min_range;
            {
                std::size_t min_size = std::numeric_limits<std::size_t>::max();
                for(component_typeinfo component_type : contains_range){
                    subrange range(components.equal_range(component_type));
                    const std::size_t range_size = range.size();

                    if (range_size < min_size){
                        min_size = range_size;
                        min_range = range;
                    }
                }
            }

            // 2. Traverse it
            for(auto& key_value : min_range){
                components_container& component = *key_value.second;
                if (!check_container(component)) continue;

                const bool proceed = closure(component);
                if (!proceed) break;
            }
        }

    // -----------------------------------
    //           DATA
    // -----------------------------------
    private:
        class entity_manager entity_manager;
        std::unordered_map<archetype_typeinfo, components_container> archetypes;
        std::unordered_multimap<component_typeinfo, components_container*> components;  // TODO: optimise
    };

}