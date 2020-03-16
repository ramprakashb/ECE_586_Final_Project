/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
 * Modified by: Kamali, Michael, and Ram
 * ECE 586, Winter 2020
 * Portland State University
 * Competition Predictor
*/

#include "predictor.h"

/*	Macros	*/
#define PREDICTOR_SIZE	3		// Size in bits.

/*	Global Variables	*/
unsigned int path_history;
unsigned int pc_select;
_table table[8];

bool PREDICTOR::get_prediction(const branch_record_c* br  , const op_state_c* os ){

	return prediction();
}

// Update the predictor after a prediction has been made.  This should accept
// the branch record (br) and architectural state (os), as well as a third
// argument (taken) indicating whether or not the branch was taken.
void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken){


	/*	Debug	*/	debug(0, "NEWLINE");
}

/************************************
************ Function ***************
*********** Definitions *************
************************************/

bool prediction(void){
	return	table[7].match ? (0x1 & (table[7].prediction >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[6].match ? (0x1 & (table[6].prediction >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[5].match ? (0x1 & (table[5].prediction >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[4].match ? (0x1 & (table[4].prediction >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[3].match ? (0x1 & (table[3].prediction >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[2].match ? (0x1 & (table[2].prediction >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[1].match ? (0x1 & (table[1].prediction >> (ipow(2, PREDICTOR_SIZE) -1))) :
							 (0x1 & (table[0].prediction >> (ipow(2, PREDICTOR_SIZE) -1))) ;
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

unsigned int ipow(unsigned int base, unsigned int exponent){
	unsigned int temp = base;
	for(unsigned int i = 0; i < (exponent - 1); i++)
		temp = temp * base;
	return temp;
}