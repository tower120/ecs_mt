#pragma once

#include "type_constant.hpp"
#include "integral_constant_sequence.hpp"
#include  <functional>

namespace tower120::ecs::impl::utils{

    namespace details{
        template <typename> struct is_tuple: std::false_type {};
        template <typename ...T> struct is_tuple<std::tuple<T...>>: std::true_type {};
    }

    template <class T>
    constexpr const static bool is_tuple = details::is_tuple<std::decay_t<T>>::value;

    template<int N, class Closure>
    constexpr void static_for(Closure&& closure){
        std::apply([&](auto ...integral_constants){
            (closure(integral_constants), ...);
        }, integral_constant_sequence<N>{});
    }

    template<class Closure, class ...Args>
    constexpr void foreach(Closure&& closure, Args&&...args){
        ( closure(std::forward<Args>(args)) ,  ...);
    }

    template<class Closure, class ...Args>
    constexpr void foreach(Closure&& closure, std::tuple<Args...>& tuple){
        std::apply([&](auto&... args){
            foreach(std::forward<Closure>(closure), args...);
        }, tuple);
    }

    /*template<class Arg, class ...Args, class Closure,
        typename = std::enable_if_t<!is_tuple<Arg>>>
    constexpr void foreach(Closure&& closure){
        foreach(std::forward<Closure>(closure), type_constant<Arg>{}, type_constant<Args>{}...);
    }*/

    template<class Tuple, class Closure,
        typename = std::enable_if_t<is_tuple<Tuple>>>
    constexpr void foreach(Closure&& closure){
        static_for<std::tuple_size_v<Tuple>>([&](auto integral_constant){
            using T = std::tuple_element_t<integral_constant.value, Tuple>;
            closure( type_constant<T>{} );
        });
    }

    template<class Container, class Iterator>
    void unordered_erase(Container& container, Iterator iter){
        *iter = std::move(container.back());
        container.pop_back();
    }

}