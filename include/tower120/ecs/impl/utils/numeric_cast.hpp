#pragma once

#include <type_traits>
#include <limits>
#include <cstdint>
#include <cassert>

namespace tower120::ecs::impl::utils{

    template<class T, class Num>
    T numeric_cast(Num num) {
        static_assert(std::is_integral_v<Num>);
        static_assert(std::is_integral_v<T>);

        assert(
            (
                num < 0  && static_cast<std::intmax_t>(std::numeric_limits<T>::min()) <= static_cast<std::intmax_t>(num)
            )
            ||
            (
                num >= 0 && static_cast<std::uintmax_t>(num) <= static_cast<std::uintmax_t>(std::numeric_limits<T>::max())
            )
        );

        return static_cast<T>(num);
    }

}