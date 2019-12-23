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


    template<class Container, class Iterator>
    void unordered_erase(Container& container, Iterator iter){
        *iter = std::move(container.back());
        container.pop_back();
    }

}