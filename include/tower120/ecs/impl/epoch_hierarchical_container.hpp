#pragma once

#include <tower120/ecs/impl/utils/algorithm.hpp>
#include <vector>
#include <cassert>

namespace tower120::ecs::impl{

    template<class Epoch>
    class epoch_hierarchical_container {
    // -----------------------------------------------------------
    //                   INTERFACE
    // -----------------------------------------------------------
    public:
        epoch_hierarchical_container
        (
            std::size_t group_size_step_pot,
            std::size_t level_count
        )
            : m_group_size_step_pot(group_size_step_pot)
        {
            m_levels.resize(level_count);
        }

        void push_back(const Epoch& epoch);

        void set_v1(std::size_t index, const Epoch& epoch);

        void set(std::size_t index, const Epoch& epoch);

        void unordered_erase(std::size_t index_);

        const auto& levels() const{ return m_levels; }

    // -----------------------------------------------------------
    //                      IMPLEMENTATION
    // -----------------------------------------------------------
    private:
        // optimized version
        std::optional<std::pair<Epoch, Epoch>> /*proceed*/
        update_level(std::size_t level_index, std::size_t element_index, const Epoch& old_value, const Epoch& new_value);

        // always recalculate
        std::pair<Epoch, Epoch> update_level(std::size_t level_index, std::size_t element_index);

        template<std::size_t start_level = 0>
        void update_levels(std::size_t original_index, Epoch old_value, Epoch new_value);

        void update_levels(std::size_t original_index);

    // -----------------------------------------------------------
    //                      DATA
    // -----------------------------------------------------------
    private:
        std::size_t m_group_size_step_pot;
        std::vector<std::vector<Epoch>> m_levels;
    };

} // namespace

#include "epoch_hierarchical_container.inl"