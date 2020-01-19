Query
=======

Header : ``tower120/ecs/query.hpp``

You can query your world with the following api:

.. code-block::

  world main_world;

  auto query_b = 
      query(main_world)
      .contains<ComponentA>()
      .not_contains<ComponentC>()
      .select<ComponentB>;

  for (auto&&[entity, b] : query_b)
  {
      ...
  }



.. function:: query_t query(world&)

  Construct query. *Exists as global function mainly for ADL purporses.*

----

.. class:: query_t

  .. function:: query_t(world&) noexcept
  .. function:: template<class ...Components> query_t& contains()
  .. function:: query_t& contains(component_typeinfo...components)

    Queried entities must contain all components. Chained ``contains`` accumulates:

    .. code-block::

      component_typeinfo component_b = component_typeid<ComponentB>;

      auto query_ab = 
         query(main_world)
         .contains<ComponentA>()
         .contains(component_b);

  .. function:: template<class ...Components> query_t& not_contains()
  .. function:: query_t& not_contains(component_typeinfo...components)

    Queried entities must not contain any of components. But otherwise same as :cpp:func:`query_t& contains(component_typeinfo...components)`

  .. function:: template<class Closure> void foreach_container(Closure&& closure)

    .. code-block::    

      query(main_world)
      .contains<ComponentA>()
      .foreach_container([](container_view container) -> bool /*proceed*/{
        ...
        return true;
      });

    if ``closure`` return false - iteration will break.

  .. function:: template<class Closure> void foreach(Closure&& closure)

    Required components deducted from ``closure`` signature:

    .. code-block::    

      query(main_world)
      .foreach([](/*entity*/, ComponentA&, ComponentB&) -> bool /*proceed / optional*/{
        ...
        return true; /* optional */
      });

    ``closure`` may also accept entity as first argument.

    ``closure`` may return void, or bool. If ``closure`` return false - iteration will break.

  .. function:: template<class ...Components> query_result<Components...> select()

    .. code-block::    

      for (auto&&[entity, a, b] : query(main_world).select<ComponentA, ComponentB>())
      {
          ...
      }

----

.. class:: template<class ...Components> query_result

  for loop friendly Forward Range. iterator dereferences to ``std::tuple<entity, Components&...>``.

  .. function:: span<container_view> containers() const

  .. function:: iterator begin()

  .. function:: iterator end()