/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
*/p

#include "predictor.h"

#define LHTADDRMASK 2047 // Selects bits 0-9
#define LHTHEIGHT 1024
#define LHTWIDTH 10
#define LPTHEIGHT 1024
#define LPTWIDTH 3
#define GPTHEIGHT 4096
#define GPTWIDTH 2
#define CPHEIGHT 4096
#define CPWIDTH 2


 unsigned int local_history_table[(LHTHEIGHT-1):0];
 unsigned int local_prediction_table[(LPTHEIGHT-1):0];
 unsigned int global_prediction_table[(GPTHEIGHT-1):0];
 unsigned int choice_prediction_table[(CPHEIGHT-1):-];
 unsigned int path_history;

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
		int pc;	
		//Choice Prediction


		
		//Local Predictor

		//local history table
        pc = ((br->instruction_addr)>>2) & LHTADDRMASK; //PC[11:2]
        //get the 10 bit value from the local history table ->i/p: PC value o/p:10 bit value indicating the local_history
        //local_history_table()

        //get the local prediction state value from the local predictor table: i/p: 10 bit value calculated above, o/p: state values in the table
        //local_predictor();

        //get the path history Path_history[11:0]: o/p: 12 bit path history value in the table
        //get_path_history();

		//Global Predictor: i/p: return value from the get_path_history(), o/p: 2 bit global state output.
		//global_predictor();


        //Choice predictor: i/p: return value from the get_path_history(), o/p: choice predicted.
        //choice_predictor()


        //mux logic: i/p: output of the local_predictor() function, output of the global_predictor(), output of hte path_history fuction function o/p: branch predicted value 
        //mux_logic

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

        //update local predictor: i/p: output of the local_history_table() function, 
        //update_local_predictor

		// Local Table Update: i/p: pc[11:0] 
        //update_local_history_table();
        
        //i/p: output of the get_path_history() function, 
        //update_global_predictor();
        
        //i/p: output of the get_path_history() function.
        //update_choice_predictor

        //update path_history(): i/p: taken
        //update_path_history(taken);


        }

	