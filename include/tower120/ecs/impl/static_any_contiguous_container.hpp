#pragma once

#include <vector>
#include <cstddef>
#include <typeindex>
#include <cassert>

namespace tower120::ecs::impl{

    struct any_contiguous_container_info final{
        using copy_ctr_t = void(*)(void* any_container_self, const void* any_container_other);
        const copy_ctr_t copy_ctr;

        using move_ctr_t = void(*)(void* any_container_self, void* any_container_other);
        const move_ctr_t move_ctr;

        using size_t = std::size_t(*)(const void* any_container);
        const size_t size;

        using unorderd_erase_t = void(*)(void* any_container, std::size_t index);
        const unorderd_erase_t unordered_erase;

        using unordered_move_back_t = void(*)(void* any_container_from, void* any_container_to, std::size_t index);
        const unordered_move_back_t unordered_move_back;

        using destructor_t = void(*)(void* any_container);
        const destructor_t destructor;
    };


    template<std::size_t storage_size>
    class static_any_contiguous_container{
    // ---------------------------------------------
    //              INTERFACE
    // ---------------------------------------------
    public:

        static_any_contiguous_container(const static_any_contiguous_container& other) noexcept
            : info(other.info)
            #ifndef NDEBUG
            , type_index{other.type_index}
            #endif
        {
            info.copy_ctr(&storage, &other.storage);
        }


        static_any_contiguous_container(static_any_contiguous_container&& other) noexcept
            : info(other.info)
            #ifndef NDEBUG
            , type_index{other.type_index}
            #endif
        {
            info.move_ctr(&storage, &other.storage);
        }

        template<class Container>
        explicit static_any_contiguous_container(Container container)
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

        void unordered_move_back(static_any_contiguous_container& to, std::size_t index){
            info.unordered_move_back(&storage, &to.storage, index);
        }

        ~static_any_contiguous_container(){
            info.destructor(&storage);
        };
    // ---------------------------------------------
    //              IMPLEMENTATION
    // ---------------------------------------------
    private:
        template<class Container>
        static const any_contiguous_container_info& construct_info(){
            constexpr const static any_contiguous_container_info info{
                /*.copy_ctr = */ [](void* any_container_self, const void* any_container_other){
                    Container& container_self  = *static_cast<Container*>(any_container_self);
                    const Container& container_other = *static_cast<const Container*>(any_container_other);
                    new (&container_self) Container (container_other);
                },
                /*.move_ctr = */ [](void* any_container_self, void* any_container_other){
                    Container& container_self  = *static_cast<Container*>(any_container_self);
                    Container& container_other = *static_cast<Container*>(any_container_other);
                    new (&container_self) Container (std::move(container_other));
                },
                /*.size = */ [](const void* any_container) -> std::size_t{
                    const Container& container = *static_cast<const Container*>(any_container);
                    return container.size();
                },
                /*.unordered_erase = */ [](void* any_container, std::size_t index){
                    Container& container = *static_cast<Container*>(any_container);
                    container[index] = std::move(container.back());
                    container.pop_back();
                },
                /*.unordered_move_back = */ [](void* any_container_from, void* any_container_to, std::size_t index){
                    Container& container_from = *static_cast<Container*>(any_container_from);
                    Container& container_to   = *static_cast<Container*>(any_container_to);

                    container_to.push_back(std::move(container_from[index]));
                    info.unordered_erase(any_container_from, index);
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
        std::byte storage[storage_size];    // no need for alignment - will be aligned by followed class members
        const any_contiguous_container_info& info;
        #ifndef NDEBUG
        std::type_index type_index;
        #endif
    };


    using any_vector = static_any_contiguous_container<sizeof(std::vector<int>)>;

}