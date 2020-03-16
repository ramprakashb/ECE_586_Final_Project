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

void debug(unsigned int val, const char *tag);                  // For Debugging.

unsigned int ipow(unsigned int base, unsigned int exponent);    // For ease-of-use with power of two.

bool prediction(void);

void initialize(const branch_record_c* br );

void hash(void);

void tag_compare(void);

void tag_replace(void);

void plru_update(void);

void update_predictors(bool taken);

void update_path_history(bool taken);

#endif // PREDICTOR_H_SEEN

