#pragma once

#include <tuple>

namespace tower120::ecs{

    template<class ...Components>
    class archetype{
    public:
        using components_t = std::tuple<Components...>;
    };

}