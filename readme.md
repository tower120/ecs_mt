[![Build Status](https://travis-ci.org/tower120/ecs_mt.svg?branch=master)](https://travis-ci.org/tower120/ecs_mt)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/tower120/ecs_mt?branch=master&svg=true)](https://ci.appveyor.com/api/projects/status/github/tower120/ecs_mt?branch=master&svg=true)

Header only multithread-oriented ECS (entity-component-system) library. C++17

Tries to follow Unity's ECS archetype data struct (components grouped by "entity's type").

Work in progress....

### Tested on compilers:
 * GCC 7.3
 * CLANG 7.0
 * MSVC 2019

### known issues

 * library may not work yet across shared(dll) boundaries.
