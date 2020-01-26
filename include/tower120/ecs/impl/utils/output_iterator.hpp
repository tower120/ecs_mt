#pragma once

#include <utility>
#include <iterator>

namespace tower120::ecs::impl::utils{

    /// Follow std::insert_iterator interface
    /// #https://en.cppreference.com/w/cpp/iterator/insert_iterator
    template<class Closure>
    class output_iterator{
    public:

        using iterator_category = std::output_iterator_tag;
        using value_type = void;
        using difference_type =	void;
        using pointer = void;
        using reference = void;

        constexpr explicit output_iterator(Closure closure)
            : closure( std::move(closure) )
       {}

       template<class Value>
       constexpr output_iterator& operator=(Value&& value){
            closure(std::forward<Value>(value));
            return *this;
        }

        constexpr output_iterator& operator*()  noexcept { return *this; }
        constexpr output_iterator& operator++() noexcept { return *this; }
        constexpr output_iterator& operator++( int ) noexcept{ return *this; }

    private:
        Closure closure;
    };

}