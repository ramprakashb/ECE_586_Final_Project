/* Author: Mark Faust
 *
 * C version of predictor file
*/

#ifndef PREDICTOR_H_SEEN
#define PREDICTOR_H_SEEN

#include <cstddef>
#include <cstring>
#include <inttypes.h>
#include <vector>
#include "op_state.h"   // defines op_state_c (architectural state) class 
#include "tread.h"      // defines branch_record_c class

class PREDICTOR
{
public:
    bool get_prediction(const branch_record_c* br, const op_state_c* os);

    void update_predictor(const branch_record_c* br, const op_state_c* os, bool taken);

};

typedef struct {
    unsigned int prediction;    // Prediction bits.
    unsigned int tag;           // Tag bits.
    unsigned int path_mask;     // Used for masking path history prior to hash.
    unsigned int hash;          // The hash result of PC with masked path history.
    unsigned int match;         // The flag for a mathching tag with the hash.
}_table;

void debug(unsigned int val, const char *tag);                  // For Debugging.

unsigned int ipow(unsigned int base, unsigned int exponent);    // For ease-of-use with power of two.

bool prediction(void);

void initialize(const branch_record_c* br );

void hash(void);

void tag_compare(void);

#endif // PREDICTOR_H_SEEN

