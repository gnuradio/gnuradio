#ifndef INCLUDED_BLOCKS_PATTERNED_INTERLEAVER_H
#define INCLUDED_BLOCKS_PATTERNED_INTERLEAVER_H

#include <blocks/api.h>
#include <gr_block.h>

namespace gr {
  namespace blocks {

    class BLOCKS_API patterned_interleaver : virtual public gr_block
    {
    public:
      
      typedef boost::shared_ptr<patterned_interleaver> sptr;

      static sptr make(size_t itemsize, std::vector<int> pattern);
    };

  }
}

#endif

