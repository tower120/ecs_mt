#pragma once

#include "utils/static_any.hpp"
#include <vector>

namespace tower120::ecs::impl{
    namespace components_container_types{
        template<class T>
        using component_list_t = std::vector<T>;

        using type_erased_component_list_t = static_any<sizeof(component_list_t<int>)>;
    }
}