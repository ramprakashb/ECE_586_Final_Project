/* Author: Chris Wilkerson;   Created: Thu Aug 12 16:19:58 PDT 2004
 * Description: Branch predictor driver.
*/

#include <cstdio>
#include <cstdlib>
#include "tread.h"

// include and define the predictor
#include "predictor.h"
PREDICTOR predictor;

// usage: predictor <trace>
int
main(int argc, char* argv[])
{
    using namespace std;

    if (2 != argc) {
        printf("usage: %s <trace>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FILE *tracefile = NULL;

    if( (tracefile = fopen(argv[1], "r") ) == NULL) printf("Open File Failed----!\n");

    bool actual_taken;

    //cbp_trace_reader_c cbptr = cbp_trace_reader_c(argv[0]);
    branch_record_c br;

    char x[256] = {0};
    char y[256] = {0};

   //printf("%-12s\t", "Trace File");
   //printf("%-12s\t", "Instruction Address");
   //printf("%-12s\n", "Taken");

    // read the trace, one branch at a time, placing the branch info in br
    while (fscanf(tracefile, "%s %s\n", &x, &y) != EOF) {

        //printf("%-12s\t", x);

        br.instruction_addr = atoi(x);

        //printf("%-18u\t", br.instruction_addr);

        actual_taken = 0x1 && atoi(y);

        //printf("%-12u\n", actual_taken);

        // ************************************************************
        // Competing predictors must have the following methods:
        // ************************************************************

        // get_prediction() returns the prediction your predictor would like to make
        bool predicted_taken = predictor.get_prediction(&br, NULL);

        // predict_branch() tells the trace reader how you have predicted the branch
        /* bool actual_taken    = cbptr.predict_branch(predicted_taken); */
            
        // finally, update_predictor() is used to update your predictor with the
        // correct branch result
        predictor.update_predictor(&br, NULL, actual_taken);
    }
}



