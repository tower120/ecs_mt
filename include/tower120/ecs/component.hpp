#pragma once

#include <cstdint>
#include <tower120/ecs/impl/static_any_contiguous_container.hpp>

namespace tower120::ecs{

    namespace impl{
        class components_container;
    }
    namespace details{
        template <class Component>
        class component_info;
    }


    namespace details{
        struct component_type_data{
            using make_any_vector_t = impl::any_vector(*)();
            make_any_vector_t make_any_vector;
        };
    }


    class component_typeinfo{
        template <class Component>
        friend class details::component_info;

        template <class>
        friend struct std::hash;

        friend impl::components_container;

        explicit constexpr component_typeinfo(const details::component_type_data& type_data) noexcept
            : m_type_data(&type_data)
        {}
    public:
        // Comparison operators
        [[nodiscard]] constexpr bool operator==(const component_typeinfo& other) const noexcept {
            return m_type_data == other.m_type_data;
        }
        [[nodiscard]] constexpr bool operator!=(const component_typeinfo& other) const noexcept {
            return m_type_data != other.m_type_data;
        }
        [[nodiscard]] constexpr bool operator<(const component_typeinfo& other) const noexcept {
            return m_type_data < other.m_type_data;
        }
        [[nodiscard]] constexpr bool operator>(const component_typeinfo& other) const noexcept {
            return m_type_data > other.m_type_data;
        }
        [[nodiscard]] constexpr bool operator<=(const component_typeinfo& other) const noexcept {
            return m_type_data <= other.m_type_data;
        }
        [[nodiscard]] constexpr bool operator>=(const component_typeinfo& other) const noexcept {
            return m_type_data >= other.m_type_data;
        }
    private:
        [[nodiscard]] constexpr const details::component_type_data& type_data() const noexcept {
            return *m_type_data;
        }
    private:
        const details::component_type_data* m_type_data;
    };


    namespace details{
        // pointer to static constexpr may work wrong across dll boundaries
        // since each dll have its own set of statics. Initialization order and references may be not the same
        // need more stable way to get component id.
        // Solution 1:
        //   pass ID as template argument and use it
        // Solution 2:
        //   Use hash from __FUNC_SIG__ / __FUNCTION___ ?
        template<class Component>
        class component_info{
        private:
            const constexpr static component_type_data type_data{
                 /*.make_any_vector = */ [](){
                     return impl::any_vector{ std::vector<Component>{} };
                 }
            };
        public:
             static constexpr const component_typeinfo type{type_data};
        };
    }

    template<class Component>
    constexpr const component_typeinfo component_typeid = details::component_info<Component>::type;
}

namespace std{
    template<>
    struct hash<tower120::ecs::component_typeinfo>{
        std::size_t operator()(const tower120::ecs::component_typeinfo& component) const noexcept {
            return std::hash<const void*>{}(component.m_type_data);
        }
    };
}
