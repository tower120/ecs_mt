#pragma once

#include "entity.hpp"
#include "impl/components_container.hpp"

namespace tower120::ecs{

    class world {
    // -----------------------------------
    //            TYPES
    // -----------------------------------
        using components_container = impl::components_container;
    // -----------------------------------
    //          INTERFACE
    // -----------------------------------
    public:
        template<class ...Components>
        entity make_entity(Components... components){
            entity entity = entity_manager.make();

            // search appropriate archetype
            using Archetype = archetype<Components...>;
            auto found = archetypes.find(Archetype::typeinfo);
            components_container* container;
            if (found == archetypes.end()){
                container = &make_archetype_container(Archetype::typeinfo);
            } else {
                container = &found->second;
            }

            // emplace
            container->emplace(entity, std::move(components)...);

            return entity;
        }

    // -----------------------------------
    //          IMPLEMENTATION
    // -----------------------------------
    private:
        components_container& make_archetype_container(archetype_typeinfo archetype){
            // construct container per se
            auto pair = archetypes.emplace(archetype, archetype);   assert(pair.second);
            components_container& components_container = pair.first->second;

            // fill components matrix
            for(component_typeinfo component_type : archetype.components()){
                components.emplace(component_type, &components_container);
            }

            return components_container;
        }

    // -----------------------------------
    //           DATA
    // -----------------------------------
    private:
        entity_manager entity_manager;
        std::unordered_map<archetype_typeinfo, components_container> archetypes;
        std::unordered_multimap<component_typeinfo, components_container*> components;
    };

}