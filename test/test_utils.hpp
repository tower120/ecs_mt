#pragma once

#include <iostream>
#include <algorithm>

namespace detail{
    extern void require_failed(char const *expr, char const *file, int line){
        std::cerr
            << file << ':' << line
            << " REQUIRE \"" << expr << "\" " << "failed"
            << std::endl;
        std::abort();
    }
}

# define REQUIRE(...)				\
  ((__VA_ARGS__)					\
   ? (void)(0)						\
   : ::detail::require_failed (#__VA_ARGS__, __FILE__, __LINE__))

template<class T, class R>
void REQUIRE_EQUAL(T&& rng, std::initializer_list<R>&& list){
    const bool equal = std::equal(rng.begin(), rng.end(), list.begin());
    REQUIRE(equal);
}

template<class T, class R>
void REQUIRE_EQUAL(T&& rng, R&& list){
    const bool equal = std::equal(rng.begin(), rng.end(), list.begin());
    REQUIRE(equal);
}

template<class T, class R>
void REQUIRE_CONTAINS(const T& rng, const std::initializer_list<R>& list){
    for(const R& value : list){
        const auto found = std::find(rng.begin(), rng.end(), value);
        REQUIRE(found != rng.end());
    }
}

template<class T, class R>
void REQUIRE_SET_EQUAL(const T& rng, const std::initializer_list<R>& list){
    // O(n^2) but requires only ==
    for(const R& value : list){
        const auto values_in_list = std::count(list.begin(), list.end(), value);
        const auto values_in_rng  = std::count(rng.begin(), rng.end(), value);
        REQUIRE(values_in_list == values_in_rng);
    }
}
