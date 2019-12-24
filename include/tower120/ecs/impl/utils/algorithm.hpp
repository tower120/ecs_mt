#pragma once

#include  <functional>

namespace tower120::ecs::impl::utils{

    template<class Closure, class ...Args>
    void foreach(Closure&& closure, Args&&...args){
        (std::invoke(std::forward<Closure>(closure), args), ...);
    }

    template<class Closure, class ...Args>
    void foreach(Closure&& closure, std::tuple<Args...>& tuple){
        std::apply([&](auto&... args){
            foreach(std::forward<Closure>(closure), args...);
        }, tuple);
    }

    namespace details{
        template<class T, T... Ints>
        constexpr auto integral_sequence_fn(std::integer_sequence<T, Ints...>){
            return std::tuple< std::integral_constant<T,Ints>... >{};
        }

        template<class T, T N>
        using integral_constant_sequence = decltype(integral_sequence_fn(std::make_integer_sequence<T, N>{}));
    }

    template<std::size_t N>
    using integral_constant_sequence = details::integral_constant_sequence<std::size_t, N>;

    template<class ...Ts>
    using integral_constant_sequence_for = details::integral_constant_sequence<std::size_t, sizeof...(Ts)>;



    template<class Container, class Iterator>
    void unordered_erase(Container& container, Iterator iter){
        *iter = std::move(container.back());
        container.pop_back();
    }

}