#pragma once

#include "type_constant.hpp"

namespace tower120::ecs::impl::utils{

    template<class Functor>
    class functor_type_info{

        template<class R, class ...Args>
        constexpr static type_constant< std::tuple<Args...> > get_args_types(R (Functor::*)(Args...) const) noexcept {
            return {};
        }
        template<class R, class ...Args>
        constexpr static type_constant< std::tuple<Args...> > get_args_types(R (Functor::*)(Args...)) noexcept {
            return {};
        }

        template<class R, class ...Args>
        constexpr static type_constant<R> get_result_type(R (Functor::*)(Args...) const) noexcept {
            return {};
        }
        template<class R, class ...Args>
        constexpr static type_constant<R> get_result_type(R (Functor::*)(Args...)) noexcept {
            return {};
        }

    public:
        using Arguments = typename decltype(get_args_types(&Functor::operator()))::type;
        using Result    = typename decltype(get_result_type(&Functor::operator()))::type;
    };


    template<class Functor>
    using functor_arguments = typename functor_type_info<Functor>::Arguments;

    template<class Functor>
    using functor_result    = typename functor_type_info<Functor>::Result;
}