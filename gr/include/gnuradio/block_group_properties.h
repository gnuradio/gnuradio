#pragma once

#include <gnuradio/block.h>
#include <vector>

namespace gr {

class block_group_properties
{
public:
    block_group_properties(const std::vector<block_sptr>& blocks,
                           const std::string& name = "",
                           const std::vector<unsigned int>& affinity_mask = {})
        : _blocks(blocks), _name(name), _affinity_mask(affinity_mask)
    {
        if (_name.empty()) {
            _name = blocks[0]->alias();
        }

        if (!_affinity_mask.empty()) {
            _affinity_set = true;
        }
    }

    /*!
     * \brief Set the thread's affinity to processor core n.
     *
     * \param mask a vector of ints of the core numbers available to this block.
     */
    void set_processor_affinity(const std::vector<unsigned int>& mask);

    /*!
     * \brief Remove processor affinity to a specific core.
     */
    void unset_processor_affinity();

    /*!
     * \brief Get the current processor affinity.
     */
    std::vector<unsigned int> processor_affinity() { return _affinity_mask; }


    /**
     * @brief Get the vector of blocks
     *
     * @return std::vector<block_sptr>&
     */
    std::vector<block_sptr>& blocks() { return _blocks; }

    /**
     * @brief Return the name of the block group
     *
     * @return std::string
     */
    const std::string& name() { return _name; }

private:
    std::vector<block_sptr> _blocks;
    std::string _name;
    bool _affinity_set = false;
    std::vector<unsigned int> _affinity_mask;
};

} // namespace gr