#pragma once

#include <tower120/ecs/impl/utils/algorithm.hpp>
#include <tower120/ecs/impl/utils/output_iterator.hpp>
#include <tower120/ecs/impl/utils/numeric_cast.hpp>
#include "impl/utils/static_vector.hpp"
#include "impl/utils/tuple.hpp"
#include "component.hpp"

#include <tuple>
#include <algorithm>
#include <cstring>
#include <string_view>
#include <array>

namespace tower120::ecs{

    template <class ...Components> class archetype;

    // TODO : unique archetype_id + archetype_manager. Can be created only from main thread ? What for?
    class archetype_typeinfo{
        template <class ...Components> friend class archetype;
    // -----------------------------------------------------
    //                      INTERFACE
    // -----------------------------------------------------
    private:
        archetype_typeinfo() = default;

    public:
        archetype_typeinfo(std::initializer_list<component_typeinfo> range) noexcept
        {
            init_list(std::move(range));
            m_hash = make_hash();
        }

        static const constexpr std::size_t max_components = 8;
        using components_t = chobo::static_vector<component_typeinfo, max_components>;

        [[nodiscard]]
        const components_t& components() const noexcept { return list; }

        /// Component MUST EXISTS
        [[nodiscard]]
        std::size_t component_index(component_typeinfo component_type) const noexcept {
            const auto found = std::lower_bound(list.begin(), list.end(), component_type);
            assert(found != list.end() && *found == component_type);
            const auto index = std::distance(list.begin(), found);
            assert(index>=0);
            return impl::utils::numeric_cast<std::size_t>(index);
        }

        /// Components MUST EXISTS
        template<class ...Components>
        [[nodiscard]]
        std::array<std::size_t, sizeof...(Components)> components_indexes() const noexcept {
            using namespace impl::utils;
            std::array<std::size_t, sizeof...(Components)> out;
            std::size_t out_index = 0;
            set_to_set_map(
                components(),
                archetype<Components...>::typeinfo.components(),
                output_iterator([&](auto iter_pair) {
                    const std::size_t index = numeric_cast<std::size_t>(std::distance(list.begin(), iter_pair.first));
                    out[out_index] = index;
                    ++out_index;
                })
            );
            assert(out.size() == sizeof...(Components));
            return out; // NRVO
        }

        [[nodiscard]]
        bool contains(component_typeinfo component_type) const noexcept {
            return std::binary_search(list.begin(), list.end(), component_type);
        }

        // archetype "arithmetic"
        [[nodiscard]]
        archetype_typeinfo operator+(const archetype_typeinfo& other) const noexcept {
            using namespace impl::utils;
            archetype_typeinfo out;
            std::merge(
                this->list.begin(), this->list.end(),
                other.list.begin(), other.list.end(),
                output_iterator([out_list = &out.list](component_typeinfo comp){
                    // Do not copy duplicates
                    if (!out_list->empty() && out_list->back() == comp) return;
                    out_list->emplace_back(std::move(comp));
                })
            );
            out.m_hash = out.make_hash();
            return out;
        }

        [[nodiscard]]
        archetype_typeinfo operator-(const archetype_typeinfo& other) const noexcept {
            archetype_typeinfo out;
            std::set_difference(
                this->list.begin(), this->list.end(),
                other.list.begin(), other.list.end(),
                std::back_inserter(out.list));
            out.m_hash = out.make_hash();
            return out;
        }

        [[nodiscard]]
        bool operator==(const archetype_typeinfo& other) const noexcept {
            const auto size = list.size();
            if (size != other.list.size()) return false;

            const auto result = std::memcmp(list.data(), other.list.data(), size * sizeof(component_typeinfo));
            return result == 0;
        }
        [[nodiscard]]
        bool operator!=(const archetype_typeinfo& other) const noexcept {
            return !(*this==other);
        }

        [[nodiscard]]
        std::size_t hash() const noexcept {
            return m_hash;
        }

    private:
        void init_list(std::initializer_list<component_typeinfo>&& range) noexcept {
            using namespace impl::utils;
            assert(std::size(range) <= max_components);

            std::copy(range.begin(), range.end(), std::back_inserter(list));
            std::sort(list.begin(), list.end());

            // remove duplicates
            {
                auto iter = std::unique(list.begin(), list.end());
                list.resize( numeric_cast<std::size_t>(std::distance(list.begin(), iter)) );
            }
        }
        std::size_t make_hash() const noexcept {
            return std::hash<std::string_view>{}(
                std::string_view(reinterpret_cast<const char *>(list.data()), list.size()* sizeof(component_typeinfo))
            );
        }
    // -----------------------------------
    //           DATA
    // -----------------------------------
    private:
        components_t list;
        std::size_t m_hash;
    };


    template<class ...Components>
    class archetype {
    public:
        using components = std::tuple<Components...>;

        static_assert(impl::utils::is_unique_tuple_types<components>(),
            "archetype Components... must not have duplicates!");

        template<class Component>
        [[nodiscard]]
        static std::size_t component_index() noexcept {
            using namespace impl::utils;
            constexpr const std::size_t i = tuple_index<Component, components>();
            return std::get<i>(indices());
        }
        inline static const class archetype_typeinfo typeinfo{component_typeid<Components>...};

        // implicit cast
        operator const archetype_typeinfo&() const noexcept { return typeinfo; }
    private:
        static auto make_indices() noexcept {
            std::array<std::size_t, sizeof...(Components)>
            indices{
                typeinfo.component_index(component_typeid<Components>)...
            };
            return indices;
        }

        static const auto& indices() noexcept {
            static const auto ic = make_indices();
            return ic;
        }
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
    struct hash<tower120::ecs::archetype_typeinfo>{
        std::size_t operator()(const tower120::ecs::archetype_typeinfo& archetype) const noexcept {
            return archetype.hash();
        }
    };
}
