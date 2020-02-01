Component
==========

Component in the context of pure ECS is just a data chunk.
Components of the same :doc:`archetype` stored in the same contiguous container.
Components order is undefined.
Components may be moved during entity destruction, also when entity's archetype changes.

Library allow you to have any **movable** class/struct as component:

.. code-block::

    struct Pos {
        float x;
        float y;
    }

    class Velocity {
    public:
        Velocity(float velocity) : m_velocity(velocity) {}
        float velocity() const { return m_velocity; }
    private:
        float m_velocity;
    }

    world main_world;
    main_world.make_entity(Pos{1,2}, Velocity(15));

----

.. class:: component_typeinfo

    Runtime component typeinfo. Comparable.

.. var:: template<class Component> constexpr component_typeinfo component_typeid

    .. code-block::

        assert(component_typeid<Pos> != component_typeid<Velocity>);