#pragma once

#include  <functional>

namespace tower120::ecs::impl::utils{

    template<class T>
    struct type_constant{
        using type = T;
    };

}