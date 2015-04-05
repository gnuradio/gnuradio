#ifndef INCLUDED_TPC_COMMON_H
#define INCLUDED_TPC_COMMON_H

#include <vector>

namespace gr {
namespace fec { 

class tpc_common {
public:
    static int parity_counter( int symbol, int length );
    static void rsc_enc_bit(int input, int state_in, std::vector<int> g, int KK, int nn, 
                    std::vector< std::vector<int> > &output, std::vector< std::vector<int> > &nextStates);
    static void precomputeStateTransitionMatrix_RSCPoly(
        int numStates,
        std::vector<int> g, 
        int KK, 
        int nn, 
        std::vector< std::vector<int> > &output, 
        std::vector< std::vector<int> > &nextStates);
    static void rsc_tail( std::vector<int> &tail_p, std::vector<int> g, int max_states, int mm );
    static void itob(std::vector<int> &binVec, int symbol, int length);
};

}
}

#endif  /* INCLUDED_TPC_COMMON_H */
