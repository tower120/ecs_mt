#pragma once

namespace tower120::ecs::impl::utils{

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

}