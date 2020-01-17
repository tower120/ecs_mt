#pragma once

#include "entity.hpp"
#include "component.hpp"
#include "impl/components_container.hpp"
#include <tower120/ecs/impl/utils/algorithm.hpp>
#include <tower120/ecs/impl/utils/subrange.hpp>
#include <tower120/ecs/impl/utils/span.hpp>
#include <tower120/ecs/impl/utils/static_vector.hpp>
#include <tower120/ecs/impl/utils/basic_iterator.hpp>

namespace tower120::ecs{


    template<class ...GetComponents>
    class query_result_t{
        using Containers = std::vector<impl::components_container*>;        // TODO : small_vector?
    public:
        explicit query_result_t(Containers containers)
            : m_containers(containers)
        {}

        // return container view
        tcb::span<impl::components_container> containers(){
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


    template<class World, class ...GetComponents>
    class query_t{
    public:
        explicit query_t(World& w)
            : w(&w)
            , m_contains_components({component_typeid<GetComponents>...})
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
            foreach_container([&](impl::components_container& container) -> bool {
                std::tuple<GetComponents*...> components{container.components<GetComponents>().data()...};
                for(std::size_t i = 0; i < container.size(); ++i){
                    const bool proceed = closure(
                        std::tuple<entity, GetComponents&...>(
                            container.entities()[i],
                            std::get<GetComponents*>(components)[i]...
                        )
                    );
                    if (!proceed) return false;
                }
                return true;
            });
        }

        query_result_t<GetComponents...> select() {
            std::vector<impl::components_container*> components;
            foreach_container([&](impl::components_container& container) -> bool {
                components.emplace_back(&container);
                return true;
            });
            return query_result_t<GetComponents...>{std::move(components)};
        }

    private:
        World* w;
        using ComponentsInfoContainer = chobo::static_vector<component_typeinfo, 8>;
        ComponentsInfoContainer m_contains_components;
        ComponentsInfoContainer m_not_contains_components;
    };


}