#pragma once

#include <cassert>
#include <limits>

namespace tower120::ecs::impl::utils{

    template<class T, class tag>
    class monotonic_counter {
        static_assert(std::numeric_limits<T>::is_integer);
        inline static T counter = 0;
    public:
        static T get() noexcept {
            const T id = counter;
            assert(id < std::numeric_limits<T>::max());
            counter++;
            return id;
        }
    };

}