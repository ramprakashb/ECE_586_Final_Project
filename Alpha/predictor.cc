/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
 * Modified by: Kamali, Michael, and Ram
 * ECE 586, Winter 2020
 * Portland State University
 * Competition Predictor
*/

#include "predictor.h"

/*	Macros	*/
#define NUM_OF_WAYS		8
#define PREDICTOR_SIZE	3						// Size in bits.
#define WORD			2						// Used for PC Word Alignment.
#define PC_MASK			(INDEX_SIZE - 1)		// Used to mask PC bits for use with hash. 
#define PLRU_SIZE		(NUM_OF_WAYS - 1)
#define INDEX_SIZE		256
#define SET_LSB			0x1
#define PATH_HIST_MASK  1048575
#define PATH_HIST_SIZE  20
#define BS_VERBOSE

/*	Global Variables	*/
unsigned int path_history;
unsigned int pc_select;
unsigned int plru[INDEX_SIZE][PLRU_SIZE];

struct _table{
    unsigned int prediction[INDEX_SIZE];    // Prediction bits.
    unsigned int tag[INDEX_SIZE];           // Tag bits.
    unsigned int path_mask;     // Used for masking path history prior to hash.
    unsigned int hash;          // The hash result of PC with masked path history.
    unsigned int match;         // The flag for a mathching tag with the hash.
}table[NUM_OF_WAYS];

bool PREDICTOR::get_prediction(const branch_record_c* br  , const op_state_c* os ){
	
	initialize(br);
	hash();
	tag_compare();
	plru_update();
	tag_replace();
	return prediction();
}

// Update the predictor after a prediction has been made.  This should accept
// the branch record (br) and architectural state (os), as well as a third
// argument (taken) indicating whether or not the branch was taken.
void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken){
	update_predictors(taken);
	update_path_history(taken);
	/*	Debug	*/	debug(0, "NEWLINE");
}

/************************************
************ Function ***************
*********** Definitions *************
************************************/

void initialize(const branch_record_c* br ){

	pc_select = (PC_MASK & (br->instruction_addr >> WORD));

	debug(pc_select, "pc_select");	



	for( int i = 0; i < NUM_OF_WAYS; i++){
		table[i].path_mask = PATH_HIST_MASK & (ipow(2, (i+2)) - 1);
		
		table[0].path_mask = 0;

		/*	Debug	*/
		char string[] = "table[%d].path_mask";
		sprintf(string, "table[%d].path_mask", i);
		debug(table[i].path_mask, string);	
	}
}

void hash(void){
	for(int i = 0; i < NUM_OF_WAYS; i++ ){
		table[i].hash = ( pc_select ^ ((path_history) & table[i].path_mask));
	
		/*	Debug	*/
		char string[] = "table[%d].hash";
		sprintf(string, "table[%d].hash", i);
		debug(table[i].hash, string);			
	}
}

void tag_compare(void){
	for(int i = 0; i < NUM_OF_WAYS; i++){
		table[i].match = (table[i].hash == table[i].tag[table[i].hash]) ? 0x1 : 0x0;

		/*	Debug	*/
		char string[] = "table[%d].tag[table[%d].hash]";
		sprintf(string, "table[%d].tag[table[%d].hash]", i);
		debug(table[i].tag[table[i].hash], string);			
		sprintf(string, "table[%d].match", i);
		debug(table[i].match, string);			
	}
}

void plru_update(void){
	if(	table[7].match ){
		plru[table[7].hash][1] = 1; // Right
		plru[table[7].hash][2] = 1; // Right
		plru[table[7].hash][6] = 1; // Right
	}
	if(	table[6].match ){
		plru[table[6].hash][1] = 1; // Right
		plru[table[6].hash][2] = 1; // Right
		plru[table[6].hash][6] = 0; // Left
	}
	if(	table[5].match ){
		plru[table[5].hash][1] = 1; // Right
		plru[table[5].hash][2] = 0; // Left
		plru[table[5].hash][6] = 1; // Right
	}
	if(	table[4].match ){
		plru[table[4].hash][1] = 1; // Right
		plru[table[4].hash][2] = 0; // Left
		plru[table[4].hash][6] = 0; // Left
	}
	if(	table[3].match ){
		plru[table[3].hash][1] = 0; // Left 
		plru[table[3].hash][2] = 1; // Right
		plru[table[3].hash][6] = 1; // Right
	}
	if(	table[2].match ){
		plru[table[2].hash][1] = 0; // Left 
		plru[table[2].hash][2] = 1; // Right
		plru[table[2].hash][6] = 0; // Left 
	}
	if(	table[1].match ){
		plru[table[1].hash][1] = 0; // Left 
		plru[table[1].hash][2] = 1; // Right
		plru[table[1].hash][6] = 1; 
	}
	if(	table[0].match ){
		// No change, table[0] is the base predictor (no cache).
	}

	/*	Debug	*/
	debug(	
		(table[7].match << 7) 	| 
		(table[6].match << 6) 	| 
		(table[5].match << 5)	|
		(table[4].match << 4) 	| 
		(table[3].match << 3) 	| 
		(table[2].match << 2)   |
		(table[1].match << 1)   |
		(table[0].match << 0)	, 
		"All Table Matches");		
	
	for(int i = 0; i<NUM_OF_WAYS; i++){
		char string[] = "Table %d plru bits";
		sprintf(string, "Table %d plru bits", i);

		debug(	
			(plru[table[i].hash][1] << 7) 	| 
			(plru[table[i].hash][1] << 6) 	| 
			(plru[table[i].hash][1] << 5)	|
			(plru[table[i].hash][1] << 4) 	| 
			(plru[table[i].hash][1] << 3) 	| 
			(plru[table[i].hash][1] << 2)   |
			(plru[table[i].hash][1] << 1)   |
			(plru[table[i].hash][1] << 0)	, 
			string);
	}												


}

void tag_replace(void){
	for(int i = 0; i < NUM_OF_WAYS; i++){
		/*	Dynamic Variables	*/
		unsigned int table_select = 0;				// Selects table for tag replacement.
		unsigned int *p = plru[table[i].hash];	// Points to the plru bits of the row selected.

		/* Begin Replacement	*/
		if(!(table[i].hash == table[i].tag[table[i].hash])){
			
			table_select = 	( !p[0] && !p[2] && !p[6] )	?	7	:
							( !p[0] && !p[2] &&  p[6] )	?	6	:
							( !p[0] &&  p[2] && !p[5] )	?	5	:
							( !p[0] &&  p[2] &&  p[5] )	?	4	:
							(  p[0] && !p[1] && !p[4] )	?	3	:
							(  p[0] && !p[1] &&  p[4] )	?	2	:
							(  p[0] &&  p[1] && !p[3] )	?	1	:
															0	;
			
			table[table_select].tag[table[i].hash] = table[i].hash;
		}

		/*	Debug	*/
		char string[] = "table[i].tag[table[i].hash] - U";
		sprintf(string, "table[%d].tag[table[%d].hash] - U", i, i);
		debug(table_select, "Replacement Table");
		debug(table[i].tag[table[i].hash], string);	
	}
}


bool prediction(void){

	/*	Debug	*/
		for(int i = 0; i<NUM_OF_WAYS; i++){
			char string[] = "Table %d prediction bits";
			sprintf(string, "Table %d prediction bits", i);
			debug( table[i].prediction[table[i].hash], string);
		}	

		debug(	
			((0x1 & (table[7].prediction[table[7].hash] >> (PREDICTOR_SIZE -1))) << 7) 	| 
			((0x1 & (table[6].prediction[table[6].hash] >> (PREDICTOR_SIZE -1))) << 6) 	| 
			((0x1 & (table[5].prediction[table[5].hash] >> (PREDICTOR_SIZE -1))) << 5)	|
			((0x1 & (table[4].prediction[table[4].hash] >> (PREDICTOR_SIZE -1))) << 4) 	| 
			((0x1 & (table[3].prediction[table[3].hash] >> (PREDICTOR_SIZE -1))) << 3) 	| 
			((0x1 & (table[2].prediction[table[2].hash] >> (PREDICTOR_SIZE -1))) << 2)  |
			((0x1 & (table[1].prediction[table[1].hash] >> (PREDICTOR_SIZE -1))) << 1)  |
			((0x1 & (table[0].prediction[table[0].hash] >> (PREDICTOR_SIZE -1))) << 0)	, 
			"All Tables Prediction Bits");	

	return	table[7].match ? (0x1 & (table[7].prediction[table[7].hash] >> (PREDICTOR_SIZE -1))) :
			table[6].match ? (0x1 & (table[6].prediction[table[6].hash] >> (PREDICTOR_SIZE -1))) :
			table[5].match ? (0x1 & (table[5].prediction[table[5].hash] >> (PREDICTOR_SIZE -1))) :
			table[4].match ? (0x1 & (table[4].prediction[table[4].hash] >> (PREDICTOR_SIZE -1))) :
			table[3].match ? (0x1 & (table[3].prediction[table[3].hash] >> (PREDICTOR_SIZE -1))) :
			table[2].match ? (0x1 & (table[2].prediction[table[2].hash] >> (PREDICTOR_SIZE -1))) :
			table[1].match ? (0x1 & (table[1].prediction[table[1].hash] >> (PREDICTOR_SIZE -1))) :
							 (0x1 & (table[0].prediction[table[0].hash] >> (PREDICTOR_SIZE -1))) ;	
}

void update_predictors(bool taken){
	for( int i = (NUM_OF_WAYS -1); i >= 0; i--){
		if(	table[i].match ){
			if (table[i].prediction[table[i].hash] == taken){
				if(table[i].prediction[table[i].hash] < (ipow(2, PREDICTOR_SIZE) -1)) 
					table[i].prediction[table[i].hash]++;
			}
			else if(table[i].prediction[table[i].hash] > 0)
					table[i].prediction[table[i].hash]--;
			break;
		}
	}
			debug(	
			((0x1 & (table[7].prediction[table[7].hash] >> (PREDICTOR_SIZE -1))) << 7) 	| 
			((0x1 & (table[6].prediction[table[6].hash] >> (PREDICTOR_SIZE -1))) << 6) 	| 
			((0x1 & (table[5].prediction[table[5].hash] >> (PREDICTOR_SIZE -1))) << 5)	|
			((0x1 & (table[4].prediction[table[4].hash] >> (PREDICTOR_SIZE -1))) << 4) 	| 
			((0x1 & (table[3].prediction[table[3].hash] >> (PREDICTOR_SIZE -1))) << 3) 	| 
			((0x1 & (table[2].prediction[table[2].hash] >> (PREDICTOR_SIZE -1))) << 2)  |
			((0x1 & (table[1].prediction[table[1].hash] >> (PREDICTOR_SIZE -1))) << 1)  |
			((0x1 & (table[0].prediction[table[0].hash] >> (PREDICTOR_SIZE -1))) << 0)	, 
			"All Tables Prediction Bits-U");	
}

void update_path_history(bool taken){
	if(taken) 	path_history = (PATH_HIST_MASK & ((path_history << 1) | SET_LSB));
	else 		path_history = (PATH_HIST_MASK & (path_history << 1)) & (PATH_HIST_MASK -1);
	debug(path_history, "PathHistory Update");
}

void debug(unsigned int val, const char *tag){
	#ifdef	BS_VERBOSE
	FILE * fh = NULL;
	fh = fopen("log.txt", "a");
	
	char binary[13] = {0};
	static char tagname[256][256] = {'\0'};
	static char tagval[256][256] = {'\0'};
	static int i = 0;
	static unsigned char header = 0;

	if(	(strcmp(tag, "LINE") != 0)	&&
		(strcmp(tag, "Replacement Table") != 0) ){
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
				fprintf(fh, "%-32s\t", tagname[j]);
			}
			fprintf(fh, "\n");
			header = 1;
		}
		for(int j = 0; j < i; j++){
				fprintf(fh, "%-32s\t", tagval[j]);
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