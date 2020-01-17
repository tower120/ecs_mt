#pragma once

#include <utility>

namespace tower120::ecs::impl::utils{

    template<class I, class S = I>
    class subrange{
    public:
        subrange() = default;

        subrange(I i, S s) noexcept
            : iterator(std::move(i))
            , sentinel(std::move(s))
        {}

        // for old std support
        explicit subrange(std::pair<I, S> pair) noexcept
            : iterator(std::move(pair.first))
            , sentinel(std::move(pair.second))
        {}

        constexpr I begin() const noexcept {
            return iterator;
        }
        constexpr S end() const noexcept {
            return sentinel;
        }

        [[nodiscard]]
        std::size_t size() const noexcept {
            return numeric_cast<std::size_t>(std::distance(iterator, sentinel));
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return iterator == sentinel;
        }

    private:
        I iterator;
        S sentinel;
    };

}