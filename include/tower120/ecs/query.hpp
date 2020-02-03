#pragma once

#include "entity.hpp"
#include "component.hpp"
#include "world.hpp"
#include "impl/components_container.hpp"
#include <tower120/ecs/impl/utils/algorithm.hpp>
#include <tower120/ecs/impl/utils/subrange.hpp>
#include <tower120/ecs/impl/utils/span.hpp>
#include <tower120/ecs/impl/utils/static_vector.hpp>
#include <tower120/ecs/impl/utils/basic_iterator.hpp>
#include <tower120/ecs/impl/utils/functor_type_info.hpp>

namespace tower120::ecs{


    template<class ...GetComponents>
    class query_result{
        using Containers = std::vector<impl::components_container*>;        // TODO : small_vector?
    public:
        explicit query_result(Containers containers)
            : m_containers(containers)
        {}

        // return container view
        tcb::span<impl::components_container*> containers(){
            return m_containers;
        }

        class iterator : public impl::utils::basic_iterator<iterator>{
        public:
            iterator(Containers::iterator containers_iterator, Containers::iterator containers_sentinel, std::size_t index)
                : containers_iterator(containers_iterator)
                , containers_sentinel(containers_sentinel)
                , index(impl::utils::numeric_cast<uint32_t>(index))
                , size(0)
            {
                update_components();
            }

            std::tuple<entity, GetComponents&...> read() const noexcept {
                return {
                    (**containers_iterator).entities()[index],
                    std::get<GetComponents*>(components)[index]...
                };
            }

            void next() noexcept {
                ++index;
                if (index == size){
                    ++containers_iterator;
                    index = 0;
                    size  = 0;
                    update_components();
                }
            }

            bool equal(const iterator& other) const noexcept {
                return containers_iterator == other.containers_iterator
                    && index == other.index;
            }

        private:
            void update_components(){
                if (containers_iterator == containers_sentinel) return;
                size = impl::utils::numeric_cast<uint32_t>((**containers_iterator).size());
                impl::utils::static_for<sizeof...(GetComponents)>([&](auto integral_constant){
                    constexpr const std::size_t Index = integral_constant.value;
                    using T = std::tuple_element_t<Index, std::tuple<GetComponents...> >;
                    std::get<Index>(components) = (**containers_iterator).template components<T>().data();
                });
            }
        private:
            Containers::iterator containers_iterator;
            const Containers::iterator containers_sentinel;
            std::uint32_t index; std::uint32_t size;
            std::tuple<GetComponents*...> components;
        };


        iterator begin(){
            return {m_containers.begin(), m_containers.end(), 0};
        }
        iterator end(){
            return {m_containers.end(), m_containers.end(), 0};
        }

    private:
        Containers m_containers;
    };


    class query_t{
    public:
        /*query_t(const query_t&) = delete;
        query_t(query_t&&)      = delete;*/

        explicit query_t(world& w) noexcept
            : w(&w)
        {}

        template<class Component, class ...Components>
        query_t& contains(){
            return contains(
                component_typeid<Component>,
                component_typeid<Components>...
            );
        }

        template<class ...Components>
        query_t& contains(Components... components){
            static_assert((std::is_same_v<Components, component_typeinfo> && ...));
            impl::utils::foreach([&](component_typeinfo& ti){
                m_contains_components.emplace_back(ti);
            }, components...);
            return *this;
        }

        template<class Component, class ...Components>
        query_t& not_contains(){
            return not_contains(
                component_typeid<Component>,
                component_typeid<Components>...
            );
        }

        template<class ...Components>
        query_t& not_contains(Components... components){
            static_assert((std::is_same_v<Components, component_typeinfo> && ...));
            impl::utils::foreach([&](component_typeinfo& ti){
                m_not_contains_components.emplace_back(ti);
            }, components...);
            return *this;
        }

        template<class Closure>
        void foreach_container(Closure&& closure){
            w->query_foreach_container(
                m_contains_components,
                m_not_contains_components,
                std::forward<Closure>(closure));
        }

        template<class Closure>
        void foreach(Closure&& closure) {
            using namespace impl::utils;
            using GetComponentsTuple = functor_arguments<std::decay_t<Closure>>;

            /*static constexpr const bool is_void =
                std::is_same_v<void, functor_result<std::decay_t<Closure>>>;*/
            const auto fn = [&](auto&&...args) -> bool {
                if constexpr (std::is_same_v<void, functor_result<std::decay_t<Closure>>>){
                    closure(std::forward<decltype(args)>(args)...);
                    return true;
                } else {
                    return closure(std::forward<decltype(args)>(args)...);
                }
            };

            /*constexpr const bool pass_entity =
                std::is_same_v<entity,
                    std::tuple_element_t<0, GetComponentsTuple>
                >;*/
            apply_tuple<GetComponentsTuple>([&](auto type_constant, auto... type_constants){
                if constexpr (std::is_same_v<entity, std::tuple_element_t<0, GetComponentsTuple>>){
                    (void)type_constant;
                    foreach_impl<
                        std::decay_t<typename decltype(type_constants)::type>...
                    >(fn);
                } else {
                    foreach_impl<
                        std::decay_t<typename decltype(type_constant)::type>,
                        std::decay_t<typename decltype(type_constants)::type>...
                    >([&](const entity&, auto&&...args){
                        return fn(std::forward<decltype(args)>(args)...);
                    });
                }
            });
        }

        template<class ...GetComponents>
        query_result<GetComponents...> select() {
            auto p_contains_components = m_contains_components;
            impl::utils::foreach([&](const component_typeinfo& ti){
                p_contains_components.emplace_back(ti);
            }, component_typeid<GetComponents>...);

            std::vector<impl::components_container*> components;
            w->query_foreach_container(
                p_contains_components,
                m_not_contains_components,
                [&](impl::components_container& container) -> bool {
                    components.emplace_back(&container);
                    return true;
                });

            return query_result<GetComponents...>{std::move(components)};
        }

    private:
        template<class ...GetComponents, class Closure>
        void foreach_impl(Closure&& closure) {
            auto p_contains_components = m_contains_components;
            impl::utils::foreach([&](const component_typeinfo& ti){
                p_contains_components.emplace_back(ti);
            }, component_typeid<GetComponents>...);

            w->query_foreach_container(
                p_contains_components,
                m_not_contains_components,
                [&](impl::components_container& container) -> bool {
                    std::tuple<GetComponents*...> components{container.components<GetComponents>().data()...};
                    for(std::size_t i = 0; i < container.size(); ++i){
                        const bool proceed = closure(
                            container.entities()[i],
                            std::get<GetComponents*>(components)[i]...
                        );
                        if (!proceed) return false;
                    }
                    return true;
                }
            );
        }

    private:
        world* w;
        using ComponentsInfoContainer = chobo::static_vector<component_typeinfo, 8>;
        ComponentsInfoContainer m_contains_components;
        ComponentsInfoContainer m_not_contains_components;
    };

    // For ADL
    query_t query(world& w) noexcept {
        return query_t(w);
    }

}