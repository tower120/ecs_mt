#pragma once

#include <optional>

namespace tower120::ecs::impl::utils{

    namespace details{
        template<class T, class Tuple>
        constexpr std::optional<std::size_t> tuple_index(){
            std::optional<std::size_t> result;

            static_for< std::tuple_size_v<Tuple> >([&](auto integral_constant){
                constexpr const std::size_t index = integral_constant.value;
                using element_t = std::tuple_element_t<index, Tuple>;
                if constexpr (std::is_same_v<element_t, T>)
                    result = index;
            });

            return result;
        }
    }

    template<class T, class Tuple>
    constexpr std::size_t tuple_index(){
        constexpr const std::optional<std::size_t> index = details::tuple_index<T, Tuple>();
        static_assert(index.has_value());
        return *index;
    }

}