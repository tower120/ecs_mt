Header only multithread-oriented ECS (entity-component-system) library. C++17


### known issues

 * library may not work across shared(dll) boundaries.
     Mainly due to component id and used in static_any typeid.