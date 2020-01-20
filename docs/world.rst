World
=======

.. class:: world

    Immobile, default constructible.

    All operations requires only 1 move of all entity components.

    .. function:: template<class ...Components> entity make_entity(Components...)

        Construct entity with passed components. *Components accepted by value.*

    .. function:: void destroy_entity(entity)

        Destroy entity.

    TODO:

    .. function:: template<class ...Components> void add_components(entity, Components...)

        Add components to entity. Move all entity's components to another archetype container.

    .. function:: template<class ...Components> void remove_components(entity)

        Remove components from entity. Move all entity's components to another archetype container.
