#pragma once

#include <vector>
#include <cstddef>
#include <typeindex>
#include <cassert>

namespace tower120::ecs::impl{

    struct any_contiguous_array_info{
        using size_t = std::size_t(*)(const void* any_container);
        const size_t size;

        using unorderd_erase_t = void(*)(void* any_container, std::size_t index);
        const unorderd_erase_t unordered_erase;

        using destructor_t = void(*)(void* any_container);
        const destructor_t destructor;


        /*virtual void move(void* other, std::size_t index) = 0;*/
    };


    class any_contiguous_array{
    // ---------------------------------------------
    //              INTERFACE
    // ---------------------------------------------
    public:
        any_contiguous_array(const any_contiguous_array&) = delete;
        any_contiguous_array(any_contiguous_array&&)      = delete;

        template<class Container>
        explicit any_contiguous_array(Container container)
            : info{construct_info<Container>()}
            #ifndef NDEBUG
            , type_index{typeid(Container)}
            #endif
        {
            static_assert(sizeof(Container) <= storage_size);
            new (storage) Container (std::move(container));
        }

        template<class Container>
        Container& cast() noexcept {
            assert(std::type_index(typeid(Container)) == type_index);
            return reinterpret_cast<Container&>(storage);
        }

        [[nodiscard]]
        std::size_t size() const {
            return info.size(&storage);
        }

        void unordered_erase(std::size_t index){
            info.unordered_erase(&storage, index);
        }

        ~any_contiguous_array(){
            info.destructor(&storage);
        };
    // ---------------------------------------------
    //              IMPLEMENTATION
    // ---------------------------------------------
    private:
        template<class Container>
        static const any_contiguous_array_info& construct_info(){
            constexpr const static any_contiguous_array_info info{
                /*.size = */ [](const void* any_container) -> std::size_t{
                    const Container& container = *static_cast<const Container*>(any_container);
                    return container.size();
                },

                /*.unordered_erase = */ [](void* any_container, std::size_t index){
                    Container& container = *static_cast<Container*>(any_container);
                    container[index] = std::move(container.back());
                    container.pop_back();
                },

                /*.destructor = */ [](void* any_container){
                    Container& container = *static_cast<Container*>(any_container);
                    container.~Container();
                },
            };
            return info;
        }

    // ---------------------------------------------
    //              DATA
    // ---------------------------------------------
    private:
        static constexpr const std::size_t storage_size = sizeof(std::vector<int>);
        std::byte storage[storage_size];
        const any_contiguous_array_info& info;
        #ifndef NDEBUG
        std::type_index type_index;
        #endif
    };


}