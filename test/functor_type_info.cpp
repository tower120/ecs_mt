#include <tower120/ecs/impl/utils/functor_type_info.hpp>
#include "test_utils.hpp"

using namespace tower120::ecs::impl::utils;

int main(){
    auto fn = [](int, float){};

    using Args = functor_type_info<decltype(fn)>::Arguments;
    static_assert(std::is_same_v<Args, std::tuple<int, float>>);

    using R = functor_type_info<decltype(fn)>::Result;
    static_assert(std::is_same_v<R, void>);
}