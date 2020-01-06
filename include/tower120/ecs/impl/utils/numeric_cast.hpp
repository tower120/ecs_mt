#pragma once

#include <type_traits>
#include <cassert>

namespace tower120::ecs::impl::utils{

    template<class T, class Num>
    T numeric_cast(Num num) {
        static_assert(std::is_integral_v<Num>);
        static_assert(std::is_integral_v<T>);

        assert(std::numeric_limits<T>::min() <= num && num <= std::numeric_limits<T>::max());
        return static_cast<T>(num);
    }

}