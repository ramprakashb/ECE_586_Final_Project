/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
 * Modified by: Kamali, Michael, and Ram
 * ECE 586, Winter 2020
 * Portland State University
 * Competition Predictor
*/

#include "predictor.h"

/*Variables- gshare*/
#define ROWS 16*1024			
#define INDEX 0x3FFF			// 14 bits
#define SETLSB 0x1
#define HISTORY 0x3F		// 6 bits
#define PREDICTORS	0x3		// 2 bits
#define PREDICTOR_BITS	2

#define PREDICTION	0x1
#define GLOBAL		0xFFFFFFFF

//#define BS_VERBOSE

static unsigned int predictor_index = 0;
static unsigned int predictor = 0;
static unsigned int address_select = 0;
static unsigned int path_history = 0;
static unsigned int history_table[ROWS] = {0};
static unsigned int predictor_table[ROWS];
static unsigned int history_index = 0;

/*function Prototypes- gshare*/
unsigned int xor_comb(unsigned int a, unsigned int b);
bool get_predict(void);				// returs prediction
void update_global_branch_history(bool);				// updates path history
void update_predict_table(bool);						// updates predictor pointed to by hash
void get_branch_address(const branch_record_c* br);
void debug(unsigned int val, const char *tag);			// prints values in column format



bool PREDICTOR::get_prediction(const branch_record_c* br  , const op_state_c* os ){
	
	if(!br->is_conditional) return 1;
	if(br->is_call) return 1;
	if(br->is_return) return 1;
	if(os->num_ops < 2) return 1;
	
	// Select adddress bits.
	address_select = INDEX & (br->instruction_addr );

	// Hash with global history
	history_index = INDEX & ( address_select ^ ( GLOBAL & (path_history >> 18)));

	// Access predictor index
	predictor_index = history_index; //HISTORY & history_table[history_index];

	// Access predictor
	predictor = PREDICTORS & predictor_table[predictor_index];

	// Return prediction
	return PREDICTION & (predictor >> (PREDICTOR_BITS -1));

}


// Update the predictor after a prediction has been made.  This should accept
// the branch record (br) and architectural state (os), as well as a third
// argument (taken) indicating whether or not the branch was taken.
void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken){
	
	// Update history table
/*	if(taken)	history_table[history_index] = HISTORY & (history_table[history_index] << 1) | SETLSB;
	else		history_table[history_index] = HISTORY & (history_table[history_index] << 1) & (HISTORY - 1);
*/	
	// Update prediction bits
/*	if(taken){
			if(predictor_table[history_table[history_index]] < PREDICTORS ) 	predictor_table[history_table[history_index]]++;
	} else	if(predictor_table[history_table[history_index]] > 0 ) 				predictor_table[history_table[history_index]]--;
*/
	if(br->is_conditional && !br->is_return && !br->is_call  && (!(os->num_ops < 2))){
		if(taken){
				if(predictor_table[history_index] < PREDICTORS ) 		predictor_table[history_index]++;
		} else	if(predictor_table[history_index] > 0 ) 				predictor_table[history_index]--;
	}
	// Update global path_history
	if(taken) 	path_history = GLOBAL & (path_history << 1) | SETLSB;
	else		path_history = GLOBAL & (path_history << 1) & (GLOBAL - 1);

}



void debug(unsigned int val, const char *tag){
	#ifdef	BS_VERBOSE
	FILE * fh = NULL;
	fh = fopen("log.txt", "a");
	
	char binary[13] = {0};
	static char tagname[20][256] = {'\0'};
	static char tagval[20][256] = {'\0'};
	static int i = 0;
	static unsigned char header = 0;

	if(tag != "LINE"){
		for(char i=12; i > 0 ; i--){
			if((val >> (i - 1)) & 0x1) binary[11 - (i-1)] = '1';
			else binary[11 - (i -1)] = '0';
		}
	}
	else{
		sprintf(binary, "%lu", val);
	}

	if(strcmp(tag, "NEWLINE") != 0){
		 strcpy(tagname[i] , tag); strcpy(tagval[i] , binary); i++;
	}
	else{
				if(header == 0){
					for(int j = 0; j < i; j++){
						fprintf(fh, "%24s\t", tagname[j]);
					}
					fprintf(fh, "\n");
					header = 1;
				}
				for(int j = 0; j < i; j++){
						fprintf(fh, "%-24s\t", tagval[j]);
				}
				fprintf(fh, "\n");
				i = 0;
	}

	fclose(fh);
	#endif
}