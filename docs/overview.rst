:orphan:

Overview
=========

ECS MT is header only Entity Component System library with multithreading and reactivity in mind.


Code
-----

The code and issue tracker are hosted on Github: https://github.com/tower120/ecs_mt

Storage
--------

ECS MT has archetype storage architecture, namely all components grouped by their entities archetype. Where archetype is unique combination of component types. Similar storage layout used in Unity ECS. 

.. code-block:: cpp

    world{
        ArchetypeXY = X+Y
        std::vector<X>
        std::vector<Y>

        ArchetypeXYZ = X+Y+Z
        std::vector<X>
        std::vector<Y>
        std::vector<Z>
    }

This allows to extremely fast filter entities by their components. Downside of this kind of storage is that adding/removing component to already existing entity requires to move all of its components from one storage to another.

Versioning
-------------

NOT IMPLEMENTED.
Version is integer value which grows with each mutable operation.
Each world has its head/counter version.
Each component element has backing version number, which updates on each "write" operation to world version. Component elements versions stored in hierarchical structure for fast search of changed elements.

Versioning allows to implement reactive systems - systems which traverse only changed components.

Multithreading
--------------

NOT IMPLEMENTED. Run several systems at once - as a group.
