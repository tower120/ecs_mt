#pragma once

#include <cstdint>
#include <tower120/ecs/impl/static_any_contiguous_container.hpp>
#include "entity.hpp"

namespace tower120::ecs{

    struct component_type_data{
        using make_any_vector_t = impl::any_vector(*)();
        make_any_vector_t make_any_vector;
    };

    // TODO: make concrete type wrapper
    using component_typeinfo = const component_type_data*;


    // TODO:
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
         static constexpr const component_typeinfo type = &type_data;
    };

    template<class Component>
    constexpr const component_typeinfo component_typeid = component_info<Component>::type;

}