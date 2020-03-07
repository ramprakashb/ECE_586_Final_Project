/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
*/

#include "predictor.h"

#define LHTADDRMASK ‭1023‬ // Selects bits 0-9
#define LHTHEIGHT 1024
#define LHTWIDTH 10
#define LPTHEIGHT 1024
#define LPTWIDTH 3
#define GPTHEIGHT 4096
#define GPTWIDTH 2
#define PHSIZE 12
#define CPHEIGHT 4096
#define CPWIDTH 2


 unsigned int local_history_table[(LHTHEIGHT-1):0];
 unsigned int local_prediction_table[(LPTHEIGHT-1):0];
 unsigned int global_prediction_table[(GPTHEIGHT-1):0];
 unsigned int choice_prediction_table[(CPHEIGHT-1):-];
 unsigned int path_history[PHSIZE-1:0];

    bool PREDICTOR::get_prediction(const branch_record_c* br, const op_state_c* os)
        {
		/* replace this code with your own
            bool prediction = false;

			printf("%0x %0x %1d %1d %1d %1d ",br->instruction_addr,
			                                br->branch_target,br->is_indirect,br->is_conditional,
											br->is_call,br->is_return);
											
            if (br->is_conditional)
                prediction = true;
		*/
				
		//Choice Prediction
		
		//Local Predictor
		
		//Global Predictor
		
            return prediction;   // true for taken, false for not taken
        }


    // Update the predictor after a prediction has been made.  This should accept
    // the branch record (br) and architectural state (os), as well as a third
    // argument (taken) indicating whether or not the branch was taken.
    void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken)
        {
		/* replace this code with your own
			printf("%1d\n",taken);
			*/
			
		// Local Table Update	

        }

	