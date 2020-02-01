Archetype
==========

Archetype is unique combination of components. Entities with identical archetype stored in the same storage.
Entity archetype can be changed runtime (see :doc:`world`).

Archetype represented by :cpp:class:`archetype` and :cpp:class:`archetype_typeinfo`.

:cpp:class:`archetype` contains compile-time information about acrhetype components.
Also, internally contains precomputed :cpp:class:`archetype_typeinfo`, and has cast operator to it:

.. code-block::

    archetype_typeinfo arch_xy  = archetype<X, Y>{};

----

.. class:: archetype_typeinfo

    Runtime archetype. Contains array of unique component types.

    .. function:: archetype_typeinfo::archetype_typeinfo(std::initializer_list<component_typeinfo>) noexcept

        Construct ``archetype_typeinfo`` from components list. Duplicates will be omitted. O(N*log(N))

    .. function:: const components_t& components() const noexcept

        Return list of components of archetype.

    .. function:: bool contains(component_typeinfo) const noexcept

        O(log(N))

    .. function:: archetype_typeinfo operator+(const archetype_typeinfo&) const noexcept

        Add components from other archetype_typeinfo. Result components list does not contains duplicates:

        .. code-block::

            archetype_typeinfo arch_xy  = archetype<X, Y>{};
            archetype_typeinfo arch_xyz = arch_xy + archetype<Y, Z>{};
            assert(arch_xyz == archetype<X, Y, Z>{});

        O(N*2) worst

    .. function:: archetype_typeinfo operator-(const archetype_typeinfo&) const noexcept

        O(N*2) worst

    .. function:: bool operator==(const archetype_typeinfo&) const noexcept

        O(N)

    .. function:: bool operator!=(const archetype_typeinfo& other) const noexcept

        O(N)

    .. function:: std::size_t hash() const noexcept

        Return precomputed hash.

.. class:: template<typename...Components> archetype

    Compile-time archetype.
    ``Components...`` must be unique, you'll have compile time error otherwise.

    .. type:: components = std::tuple<Components...>

    .. member:: static const archetype_typeinfo type

    .. function:: operator const archetype_typeinfo&() const noexcept

        Return statically precomputed ``archetype_typeinfo``.

        .. code-block::

            archetype_typeinfo arch_xy  = archetype<X, Y>{};
