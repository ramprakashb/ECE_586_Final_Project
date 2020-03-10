/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
 * Modified by: Kamali, Michael, and Ram
 * ECE 586, Winter 2020
 * Portland State University
*/

#include "predictor.h"

#define LHTADDRMASK 1023 // Selects bits 0-9
#define GPTADDRMASK 4095 // Selects bits 0-12
#define LHTHEIGHT 1024
#define LHTWIDTH 10
#define LPTHEIGHT 1024
#define LPTWIDTH 3
#define GPTHEIGHT 4096
#define GPTWIDTH 2
#define CPHEIGHT 4096
#define CPWIDTH 2
#define LHTVALMASK 1023    ///local history table's 10 bits
#define GLOBAL_PREDICTION 1
#define LOCAL_PREDICTION 0

// Data Structures
static unsigned int local_history_table[LHTHEIGHT] = {0};
static unsigned int local_prediction_table[LPTHEIGHT] = {0};
static unsigned int global_prediction_table[GPTHEIGHT] = {0};
static unsigned int choice_prediction_table[CPHEIGHT] = {0};
static unsigned int path_history = 0;


/* Function Prototypes	*/
// update_predictor
void update_local_history_table();
void update_local_prediction_table();
void update_global_prediction_table();
void update_choice_prediction_table();
void update_path_history();

bool PREDICTOR::get_prediction(const branch_record_c* br, const op_state_c* os){

	/* Local Prediction logic	*/
	// local_history_table index		=	( LHTADDRMASK & (br->instruction_addr >>2) )
	// local_prediction_table index		=	local_history_table[( LHTADDRMASK & (br->instruction_addr >>2) )]
	// local_prediction					= 	( 0x1 & ( local_prediction_table[local_history_table[( LHTADDRMASK & (br->instruction_addr >>2) )]] >> (LPTWIDTH - 1) ) )

	/* Global Prediction Logic	*/
	// global_prediction_table index	=	path_history
	// global_prediction				= 	( 0x1 & ( global_prediction_table[path_history] >> (GPTWIDTH - 1) ) )

	/* Choice Prediction Logic	*/
	// choice_prediction_table index	=	path_history
	// choice_prediction				=	( 0x1 & ( choice_prediction_table[path_history] >> (CPWIDTH - 1) ) )

	/* Multiplexer */
	
	return	( 0x1 & ( choice_prediction_table[path_history] >> (CPWIDTH - 1) ) ) 		?	// GLOBAL_PREDICTION or LOCAL_PREDICTION ? 
			( 0x1 & ( global_prediction_table[path_history] >> (GPTWIDTH - 1) ) )		:	// GLOBAL_PREDICTION
			( 0x1 & ( local_prediction_table[local_history_table[( LHTADDRMASK & (br->instruction_addr >>2) )]] >> (LPTWIDTH - 1) ) ); // LOCAL_PREDICTION

}
    
// Update the predictor after a prediction has been made.  This should accept
// the branch record (br) and architectural state (os), as well as a third
// argument (taken) indicating whether or not the branch was taken.
void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken){
	
	/* Local Prediction Update logic	*/
	// local_history_table index		=	( LHTADDRMASK & (br->instruction_addr >>2) )
	// local_prediction_table index		=	local_history_table[( LHTADDRMASK & (br->instruction_addr >>2) )]
	// local_prediction value			= 	local_prediction_table[local_history_table[( LHTADDRMASK & (br->instruction_addr >>2) )]]
	
	if(taken) 	local_prediction_table[local_history_table[( LHTADDRMASK & (br->instruction_addr >>2) )]] == 0x7 	? local_prediction_table[local_history_table[( LHTADDRMASK & (br->instruction_addr >>2) )]]
																													: local_prediction_table[local_history_table[( LHTADDRMASK & (br->instruction_addr >>2) )]]++;
	else 		local_prediction_table[local_history_table[( LHTADDRMASK & (br->instruction_addr >>2) )]] == 0x0 	? local_prediction_table[local_history_table[( LHTADDRMASK & (br->instruction_addr >>2) )]]
																													: local_prediction_table[local_history_table[( LHTADDRMASK & (br->instruction_addr >>2) )]]--;
																												
}