#pragma once

namespace tower120::ecs::impl::utils{

    // TODO: remove it at all.

    template<class Closure, class ...Args>
    constexpr decltype(auto) invoke(Closure&& closure, Args&&...args){
        return closure(std::forward<Args>(args)...);
    }

}