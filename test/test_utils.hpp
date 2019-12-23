#pragma once

#include <iostream>

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