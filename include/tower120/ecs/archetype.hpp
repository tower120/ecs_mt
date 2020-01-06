#pragma once

#include <tower120/ecs/impl/utils/algorithm.hpp>
#include "impl/utils/static_vector.hpp"
#include "impl/utils/constexpr_sort.hpp"
#include "impl/utils/tuple.hpp"

#include <tuple>
#include <algorithm>
#include <cstring>
#include "component.hpp"

namespace tower120::ecs{

    template<class ...Components>
    class archetype_t;


    // rename to any_archetype ?
    //

    // TODO : unique archetype_id + archetype_manager. Can be created only from main thread ? What for?
    class archetype{
    public:
        static const constexpr std::size_t max_components = 8;
        using components_t = chobo::static_vector<component_type, max_components>;

        template<class Range>
        explicit archetype(Range range) noexcept
            : list(make_sorted_components_list(std::move(range)))
            , hash(make_hash())
        {}

        [[nodiscard]]
        const components_t& components() const noexcept { return list; }

        [[nodiscard]]
        std::size_t component_index(component_type component_type) const noexcept {
            const auto found = std::lower_bound(list.begin(), list.end(), component_type);
            assert(found != list.end() && *found == component_type);
            const auto index = std::distance(list.begin(), found);
            assert(index>=0);
            return impl::utils::numeric_cast<std::size_t>(index);
        }

        [[nodiscard]]
        bool contains(component_type component_type) const {
            return std::binary_search(list.begin(), list.end(), component_type);
        }

        [[nodiscard]]
        bool operator==(const archetype& other) const noexcept {
            const auto size = list.size();
            if (size != other.list.size()) return false;

            const auto result = std::memcmp(list.data(), other.list.data(), size * sizeof(component_type));
            return result == 0;
        }
        [[nodiscard]]
        bool operator!=(const archetype& other) const noexcept {
            return !(*this==other);
        }
    private:
        template<class Range>
        components_t make_sorted_components_list(Range&& range) const noexcept {
            assert(std::size(range) <= max_components);

            components_t components;
            std::copy(range.begin(), range.end(), std::back_inserter(components));
            std::sort(components.begin(), components.end());
            return components;
        }
        std::size_t make_hash() const noexcept {
            return std::hash<std::string_view>{}(
                std::string_view(reinterpret_cast<const char *>(list.data()), list.size()* sizeof(component_type))
            );
        }
    // -----------------------------------
    //           DATA
    // -----------------------------------
    private:
        components_t list;
    public:
        const std::size_t hash;
    };

    template<class ...Components>
    class archetype_t {
    public:
        using components = std::tuple<Components...>;

        template<class Component>
        [[nodiscard]] static std::size_t component_index() noexcept {
            using namespace impl::utils;
            constexpr const std::size_t i = tuple_index<Component, std::tuple<Components...>>();
            return std::get<i>(indices);
        }
        inline static const class archetype archetype{std::initializer_list<component_type>{component_type_of<Components>...}};
    private:
        inline static const std::tuple indices = []() noexcept {
            return std::tuple{
                archetype.component_index(component_type_of<Components>)...
            };
        }();
    };

// not work without component_type->id - can't <, > constexpr pointers
// #https://stackoverflow.com/questions/59513334/c-constexpr-pointer-and-constexpr-comparison
/*
    template<class ...Components>
    class archetype_t {
    private:
        static constexpr auto sort_components(){
            using namespace impl::utils;
            using components_tuple = std::tuple<Components...>;

            {
                constexpr const std::array list{Components::component_type...};
                constexpr const std::array s = constexpr_sort(list,
                    [](auto l, auto r) constexpr -> bool { return l == r; });

                (void)s;
            }

            struct pair{
                std::size_t index;
                component_type_t type;
            };

            constexpr const std::array components_array =
                std::apply([](auto... integral_constants){
                    return std::array{
                        pair{
                            integral_constants.value,
                            std::tuple_element_t<integral_constants.value, components_tuple>::component_type
                        }...
                    };
                }, integral_constant_sequence_for<Components...>{});

            constexpr const std::array sorted_components = constexpr_sort(components_array,
                [](auto l, auto r) constexpr -> bool { return l.type < r.type; });

            constexpr const type_constant sorted_tuple =
                std::apply([&](auto... integral_constants){
                    using T = std::tuple<
                                std::tuple_element_t<
                                    sorted_components[integral_constants.value].index,
                                    components_tuple
                                >...
                            >;

                    return type_constant<T>{};
                }, integral_constant_sequence_for<Components...>{});

            return sorted_tuple;
        }
    public:
       using components = typename decltype(sort_components())::type;
    };
*/
}

namespace std{
    template<>
    struct hash<tower120::ecs::archetype>{
        std::size_t operator()(const tower120::ecs::archetype& archetype) const noexcept {
            return archetype.hash;
        }
    };
}
