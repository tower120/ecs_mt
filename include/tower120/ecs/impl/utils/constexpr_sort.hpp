#pragma once


namespace tower120::ecs::impl::utils{

    template <typename T>
    constexpr void constexpr_swap(T& a, T& b)
    {
        T tmp{std::move(a)};
        a = std::move(b);
        b = std::move(tmp);
    }

    template<class Range, class Compare/* = std::less */>
    constexpr Range constexpr_sort(Range range, Compare compare){
        auto begin = range.begin();
        const auto end = range.end();
        if (1 < end - begin) {
            for (auto it = begin + 1; it != end; ++it) {
                for (auto mit = it; begin != mit && compare(mit[0], mit[-1]); --mit) {
                    constexpr_swap(mit[-1], mit[0]);
                }
            }
        }
        return range;
    }

}