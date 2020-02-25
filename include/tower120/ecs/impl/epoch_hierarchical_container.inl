#pragma once

#include <tower120/ecs/impl/utils/span.hpp>

namespace tower120::ecs::impl{

    template<class Epoch>
    void epoch_hierarchical_container<Epoch>::push_back(const Epoch &epoch) {
        // add original epoch as is
        m_levels[0].push_back(epoch);
        const std::size_t original_index = m_levels[0].size() - 1;

        // aggregate levels
        const std::size_t level_count = m_levels.size();
        for(std::size_t level_index = 1, group_size_pot = m_group_size_step_pot;
            level_index < level_count;
            ++level_index, group_size_pot += m_group_size_step_pot)
        {
            std::vector<Epoch>& level = m_levels[level_index];

            // get index for level
            const std::size_t index = original_index >> group_size_pot;  // index / 2^level.group_size_pot

            if (index >= level.size()){
                // add new element
                level.push_back(epoch);
            } else {
                // update existing element
                level[index] = std::max(level[index], epoch);
            }
        }
    }

    template<class Epoch>
    void epoch_hierarchical_container<Epoch>::set(std::size_t index, const Epoch& epoch) {
        m_levels[0][index] = epoch;

        // update levels
        const std::size_t end_level_index = m_levels.size() - 1;
        for(std::size_t level_index = 0; level_index < end_level_index; ++level_index){
            const std::size_t next_level_index = level_index+1;
            auto& next_level = m_levels[next_level_index];
            const std::size_t next_level_element_index = index >> (next_level_index * m_group_size_step_pot);
            Epoch& aggregated_epoch = next_level[next_level_element_index];
            
            if (aggregated_epoch >= epoch)
                break;
            //if (aggregated_epoch < epoch){
                aggregated_epoch = epoch;
                /*continue;
            }*/

            /*const auto& level = m_levels[level_index];
            const std::size_t level_group_size_pot = level_index * m_group_size_step_pot;
            const std::size_t level_group_size = 1 << level_group_size_pot;
            const std::size_t index_start = index >> level_group_size_pot;
            const std::size_t index_end   = std::min(index_start + level_group_size, level.size());

            const Epoch new_aggregated_epoch = *std::max_element(
                level.begin() + static_cast<std::ptrdiff_t>(index_start),
                level.begin() + static_cast<std::ptrdiff_t>(index_end));

            if (aggregated_epoch >= new_aggregated_epoch){
                break;
            }
            aggregated_epoch = new_aggregated_epoch;*/
        }
    }

    template<class Epoch>
    void epoch_hierarchical_container<Epoch>::unordered_erase(std::size_t index_) {
        // erase original
        //const Epoch original_epoch = m_levels[0][index_];
        utils::unordered_erase(m_levels[0], m_levels[0].begin() + static_cast<std::ptrdiff_t>(index_));

        const std::size_t base_level_size = m_levels[0].size();
        const std::size_t base_level_last_index = base_level_size - 1;

        // 1. pop_back excess elements in levels
        const std::size_t level_count = m_levels.size();
        for(std::size_t level_index = 1, group_size_pot = m_group_size_step_pot;
            level_index < level_count;
            ++level_index, group_size_pot += m_group_size_step_pot)
        {
            std::vector<Epoch>& level = m_levels[level_index];

            // get index for level
            const std::size_t index = base_level_last_index >> group_size_pot;  // index / 2^level.group_size_pot
            if (index != level.size() - 1){
                // remove element
                level.pop_back();
            }
        }

        // 2. update levels index
        if (index_ != base_level_size)
            update_levels(index_);
        //update_levels(index_, original_epoch, m_levels[0][index_]);

        // 3. update levels end
        update_levels(base_level_last_index);
    }

    template<class Epoch>
    std::optional<std::pair<Epoch, Epoch>>
    epoch_hierarchical_container<Epoch>::update_level(std::size_t level_index, std::size_t element_index,
                                                      const Epoch &old_value, const Epoch &new_value) {
        assert(m_levels[0].size() > element_index);

        const std::size_t next_level_index = level_index + 1;
        if (next_level_index == m_levels.size()) return {};  // last level?

        const std::size_t next_level_group_size_pot = next_level_index * m_group_size_step_pot;
        const std::size_t next_level_element_index  = element_index >> next_level_group_size_pot;
        Epoch& aggregated = m_levels[next_level_index][next_level_element_index];

        // 0.
        if (new_value == aggregated)
            return {};

        // I. new_value > aggregated
        if (new_value > aggregated){
            const Epoch old_aggregated = aggregated;
            aggregated = new_value;
            return {{old_aggregated, aggregated}};
        }

        // II. old_value == aggregated && new_value < aggregated
        if (old_value == aggregated && new_value < aggregated){
            // recalculate
            const auto& level = m_levels[level_index];
            const std::size_t group_size_pot    = next_level_group_size_pot - m_group_size_step_pot;
            const std::size_t group_start_index = element_index >> group_size_pot;  // index / 2^level.group_size_pot
            std::size_t group_end_index = (element_index + 1) >> group_size_pot;
            if (group_end_index > level.size()){
                group_end_index = level.size();
            }

            const Epoch old_aggregated = aggregated;
            aggregated = *std::max_element(
                    level.begin() + static_cast<std::ptrdiff_t>(group_start_index),
                    level.begin() + static_cast<std::ptrdiff_t>(group_end_index));

            if (new_value <= aggregated){
                return {};
            } else {
                return {{old_aggregated, aggregated}};
            }
        }

        // III. new_value < aggregate
        return {};
    }

    template<class Epoch>
    std::pair<Epoch, Epoch>
    epoch_hierarchical_container<Epoch>::update_level(std::size_t level_index, std::size_t element_index) {
        assert(m_levels[0].size() > element_index);

        const std::size_t next_level_index = level_index + 1;
        if (next_level_index == m_levels.size()) return {};  // last level?

        const std::size_t next_level_group_size_pot = next_level_index * m_group_size_step_pot;
        const std::size_t next_level_element_index  = element_index >> next_level_group_size_pot;
        Epoch& aggregated = m_levels[next_level_index][next_level_element_index];

        // recalculate
        const auto& level = m_levels[level_index];
        const std::size_t group_size_pot    = next_level_group_size_pot - m_group_size_step_pot;
        const std::size_t group_start_index = element_index >> group_size_pot;  // index / 2^level.group_size_pot
        std::size_t group_end_index = (element_index + 1) >> group_size_pot;
        if (group_end_index > level.size()){
            group_end_index = level.size();
        }

        const Epoch old_aggregated = aggregated;
        aggregated = *std::max_element(
                level.begin() + static_cast<std::ptrdiff_t>(group_start_index),
                level.begin() + static_cast<std::ptrdiff_t>(group_end_index));

        return {old_aggregated, aggregated};
    }

    template<class Epoch>
    template<std::size_t start_level>
    void
    epoch_hierarchical_container<Epoch>::update_levels(std::size_t original_index, Epoch old_value, Epoch new_value) {
        const std::size_t level_count = m_levels.size();
        for(std::size_t level_index = start_level; level_index < level_count; ++level_index)
        {
            const std::optional<std::pair<Epoch, Epoch>> proceed = update_level(level_index, original_index, old_value, new_value);
            if (!proceed) break;

            old_value = proceed->first;
            new_value = proceed->second;
        }
    }

    template<class Epoch>
    void epoch_hierarchical_container<Epoch>::update_levels(std::size_t original_index) {
        // 1. force update zero level
        const std::pair<Epoch, Epoch> proceed = update_level(0, original_index);
        // 2. update all next levels as usual
        update_levels<1>(original_index, proceed.first, proceed.second);
    }


}