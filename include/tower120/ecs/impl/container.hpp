#pragma once

#include "../entity.hpp"
#include "../archetype.hpp"
#include "archetype_container.hpp"
#include <unordered_map>

namespace tower120::ecs::impl{

    class container{
    // -----------------------------------
    //          INTERFACE
    // -----------------------------------
    public:
        template<class ...Components>
        void emplace(entity, Components...components);

        template<class ...Components>
        bool erase(entity);

    // -----------------------------------
    //           DATA
    // -----------------------------------
    private:
        std::unordered_map<archetype, archetype_container> archetypes;

    };

}