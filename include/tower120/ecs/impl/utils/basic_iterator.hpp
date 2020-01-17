#pragma once

namespace tower120::ecs::impl::utils{

    template<class Derived>
    class basic_iterator{
    public:
        [[nodiscard]]
        constexpr decltype(auto) operator*() const noexcept{
            return derived().read();
        }

        constexpr Derived& operator++() noexcept {
            derived().next();
            return derived();
        }

        constexpr Derived operator++(int) noexcept {
            return ++derived();
        }

        [[nodiscard]]
        constexpr bool operator==(const Derived& other) const noexcept{
            return derived().equal(other);
        }
        [[nodiscard]]
        constexpr bool operator!=(const Derived& other) const noexcept{
            return !derived().equal(other);
        }
    private:
        constexpr Derived& derived() noexcept {
            return *static_cast<Derived*>(this);
        }
        constexpr const Derived& derived() const noexcept {
            return *static_cast<const Derived*>(this);
        }
    };

}