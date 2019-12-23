cmake_minimum_required(VERSION 3.8)
project(ecs_mt)

set(CMAKE_CXX_STANDARD 17)

add_compile_options(-Wall -Wextra -pedantic -Werror)

if (SAN STREQUAL "ASAN")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=integer -fsanitize=nullability -fno-omit-frame-pointer -g")
elseif(SAN STREQUAL "MSAN")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fno-omit-frame-pointer -g")
endif()

include_directories(include)

file(GLOB_RECURSE INCLUDE "include/*.hpp")

# ----- BENCHMARK -------

#add_subdirectory(benchmark)

# ----- TEST -------

function(_add_test TESTNAME FIRSTSOURCE)
  add_executable(${TESTNAME} ${INCLUDE} ${FIRSTSOURCE})
  add_test(${TESTNAME} ${TESTNAME})
endfunction(_add_test)

enable_testing()
add_subdirectory(test)