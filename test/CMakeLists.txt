#include_directories(../third_party/range-v3/include)

_add_test(test.archetype archetype.cpp)
_add_test(test.components_container components_container.cpp)
_add_test(test.static_any_contiguous_container static_any_contiguous_container.cpp)
