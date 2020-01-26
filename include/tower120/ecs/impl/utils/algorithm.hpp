#pragma once

#include "type_constant.hpp"
#include "integral_constant_sequence.hpp"
#include  <functional>

namespace tower120::ecs::impl::utils{

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

    template<class Arg, class ...Args, class Closure>
    constexpr void foreach(Closure&& closure){
        foreach(std::forward<Closure>(closure), type_constant<Arg>{}, type_constant<Args>{}...);
    }


    template<class Closure, class ...Args>
    constexpr void foreach_tuple(Closure&& closure, std::tuple<Args...>& tuple){
        std::apply([&](auto&... args){
            ( closure(args),  ...);
        }, tuple);
    }

    template<class Closure, class ...Args>
    constexpr void foreach_tuple(Closure&& closure, std::tuple<Args...> tuple){
        std::apply([&](auto&... args){
            ( closure(args),  ...);
        }, tuple);
    }

    template<class Tuple, class Closure>
    constexpr void foreach_tuple(Closure&& closure){
        static_for<std::tuple_size_v<Tuple>>([&](auto integral_constant){
            using T = std::tuple_element_t<integral_constant.value, Tuple>;
            closure( type_constant<T>{} );
        });
    }


    namespace details{
        template<class Closure, class ...Args>
        constexpr void apply_tuple(Closure&& closure, type_constant<std::tuple<Args...>>){
            std::apply(
                std::forward<Closure>(closure),
                std::tuple< type_constant<Args>... >{}
            );
        }
    }

    template<class Tuple, class Closure>
    constexpr void apply_tuple(Closure&& closure){
        details::apply_tuple(std::forward<Closure>(closure), type_constant<Tuple>{});
    }

    template<class Container, class Iterator>
    void unordered_erase(Container& container, Iterator iter){
        *iter = std::move(container.back());
        container.pop_back();
    }

    template<class ContainerFrom, class ContainerTo, class IteratorFrom>
    void unordered_move_back(ContainerFrom& container_from, ContainerTo& container_to, IteratorFrom iter_from){
        container_to.push_back(std::move(*iter_from));
        unordered_erase(container_from, std::move(iter_from));
    }


    /// map Range1 => Range2
    /// Return std::pair<First1, First2>
    /// Range1, Range2 must be sorted
    template<
        class First1, class Last1,
        class First2, class Last2,
        class OutputIterator>
    OutputIterator set_to_set_map(
            First1 first1, Last1 last1,
            First2 first2, Last2 last2,
            OutputIterator out)
    {
        /// based on std::set_intersection algorithm
        while (first1 != last1 && first2 != last2) {
            if (*first1 < *first2) {
                ++first1;
            } else  {
                if (!(*first2 < *first1)) {
                    ++out;
                    *out = std::pair<First1, First2>{first1, first2};
                    ++first1;
                }
                ++first2;
            }
        }

        return out;
    }

    template<
        class Range1,
        class Range2,
        class OutputIterator>
    OutputIterator set_to_set_map(
            Range1&& range1,
            Range2&& range2,
            OutputIterator out)
    {
        return set_to_set_map(
            range1.begin(), range1.end(),
            range2.begin(), range2.end(),
            std::move(out));
    }


    /*template<class Key, class ... Args>
    bool contains(const std::unordered_multimap<Key, Args...>& map, const Key& key){
        return map.find(key) != map.end();
    }*/

}