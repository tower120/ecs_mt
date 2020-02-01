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

        output_iterator(const output_iterator&) = default;
        output_iterator(output_iterator&&)      = default;

        constexpr output_iterator& operator=(const output_iterator& other) {
            if constexpr(std::is_copy_assignable_v<Closure>){
                this->closure = other.closure;
            } else {
                this->closure.~Closure();
                new (&this->closure) Closure(other.closure);
            }
            return *this;
        }

        constexpr output_iterator& operator=(output_iterator&& other) noexcept {
            if constexpr(std::is_move_assignable_v<Closure>){
                this->closure = std::move(other.closure);
            } else {
                this->closure.~Closure();
                new (&this->closure) Closure(std::move(other.closure));
            }
            return *this;
        }


       template<class Value, typename = std::enable_if_t<
           !std::is_same_v<std::decay_t<Value>, output_iterator<Closure>>
       >>
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