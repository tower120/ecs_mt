#pragma once

#include "type_constant.hpp"
#include "integral_constant_sequence.hpp"
#include "invoke.hpp"
#include  <functional>

namespace tower120::ecs::impl::utils{

    template<int N, class Closure>
    constexpr void static_for(Closure&&);


    template<class Closure, class ...Args>
    constexpr void foreach(Closure&& closure, Args&&...args){
        (utils::invoke(std::forward<Closure>(closure), args), ...);
    }

    template<class Closure, class ...Args>
    constexpr void foreach(Closure&& closure, std::tuple<Args...>& tuple){
        std::apply([&](auto&... args){
            foreach(std::forward<Closure>(closure), args...);
        }, tuple);
    }

    template<class ...Args, class Closure>
    constexpr void foreach(Closure&& closure){
        foreach(std::forward<Closure>(closure), type_constant<Args>{}...);
    }

    template<class Tuple, class Closure>
    constexpr void foreach(Closure&& closure){
        static_for<std::tuple_size_v<Tuple>>([&](auto integral_constant){
            using T = std::tuple_element_t<integral_constant.value, Tuple>;
            closure( type_constant<T>{} );
        });
    }

    template<int N, class Closure>
    constexpr void static_for(Closure&& closure){
        std::apply([&](auto ...integral_constants){
            (utils::invoke(std::forward<Closure>(closure), integral_constants), ...);
        }, integral_constant_sequence<N>{});
    }

    template<class Container, class Iterator>
    void unordered_erase(Container& container, Iterator iter){
        *iter = std::move(container.back());
        container.pop_back();
    }

}