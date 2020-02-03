[![Build Status](https://travis-ci.org/tower120/ecs_mt.svg?branch=master)](https://travis-ci.org/tower120/ecs_mt)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/tower120/ecs_mt?branch=master&svg=true)](https://ci.appveyor.com/project/tower120/ecs-mt/branch/master)
[![Documentation Status](https://readthedocs.org/projects/ecs-mt/badge/?version=latest)](https://ecs-mt.readthedocs.io/en/latest/?badge=latest)

Header only multithread-oriented ECS (entity-component-system) library. C++17

Tries to follow Unity's ECS archetype data struct (components grouped by "entity's type").

Work in progress....

[Documentation here](https://ecs-mt.readthedocs.io/)

### Tested on compilers:
 * GCC 7.3
 * CLANG 7.0
 * MSVC 2019

### known issues

 * Due to poor MSVC's `std::deque` performance, creating new entities is somewhat slow with MSVC.
 * currently MSVC is x2-x3 times slower, mainly due to poor work with lambdas.
 * library may not work yet across shared(dll) boundaries.
