#pragma once

#include <cstdint>
#include <tower120/ecs/impl/utils/monotonic_counter.hpp>

namespace tower120::ecs{

    /*template<class Component>
    uint16_t component_id() {
        const static uint16_t type_id = impl::utils::monotonic_counter<uint16_t, Component>::get();
        return type_id;
    }*/

    using component_type_id_t = uint16_t;

    // for user defined specializations
    template<class Component>
    component_type_id_t component_type_id();

    class component{};
}