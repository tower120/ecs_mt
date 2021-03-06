cmake_minimum_required(VERSION 3.8)
project(ecs_mt)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -Wconversion -pedantic -Werror)
  if (APPLE)
    add_compile_options(-Wno-error=missing-braces)
  endif()
endif()


if (SAN STREQUAL "ASAN")
  message("using ASAN")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=integer -fsanitize=nullability -fno-omit-frame-pointer -g")
elseif(SAN STREQUAL "MSAN")
  message("using MSAN")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fno-omit-frame-pointer -g")
endif()

include_directories(include)

file(GLOB_RECURSE INCLUDE "include/*.hpp")

# ----- TEST -------

function(_add_test TESTNAME FIRSTSOURCE)
  add_executable(${TESTNAME} ${INCLUDE} ${FIRSTSOURCE})
  add_test(${TESTNAME} ${TESTNAME})
endfunction(_add_test)

enable_testing()
add_subdirectory(test)

# ----- BENCHMARK -------

add_subdirectory(benchmark)
