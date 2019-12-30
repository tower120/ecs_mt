#include_directories(../third_party/range-v3/include)

#_add_test(test.archetype_container archetype_container.cpp)
_add_test(test.archetype archetype.cpp)
_add_test(test.components_container components_container.cpp)
_add_test(test.type_erased_vector any_contiguous_array.cpp)
