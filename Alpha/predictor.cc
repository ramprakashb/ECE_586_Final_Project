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
#define INDEX_SIZE		512
#define SET_LSB			0x1
#define PATH_HIST_MASK  PC_MASK
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

	table[0].path_mask = 0;

	for( int i = 1; i < NUM_OF_WAYS; i++){
		table[i].path_mask = PATH_HIST_MASK & (ipow(2, (i+2)) - 1);
	}
/*	debug(table[0].path_mask, "t0 path_mask");y
	debug(table[1].path_mask, "t1 path_mask");
	debug(table[2].path_mask, "t2 path_mask");
	debug(table[3].path_mask, "t3 path_mask");
	debug(table[4].path_mask, "t4 path_mask");
	debug(table[5].path_mask, "t5 path_mask");
	debug(table[6].path_mask, "t6 path_mask");
	debug(table[7].path_mask, "t7 path_mask");	*/
}

void hash(void){
	for(int i = 0; i < NUM_OF_WAYS; i++ ){
		table[i].hash = ( pc_select ^ (path_history & table[i].path_mask));
/*		debug(table[i].path_mask, "path_mask");	*/
	}
}

void tag_compare(void){
	for(int i = 0; i < NUM_OF_WAYS; i++){
		table[i].match = (table[i].hash == table[i].tag[table[i].hash]) ? 0x1 : 0x0;
/*		debug(table[i].tag[table[i].hash], "table tag");
		debug(table[i].match, "table_match");	*/
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
}

void tag_replace(void){
	for(int i = 0; i < NUM_OF_WAYS; i++){
		if(!(table[i].hash == table[i].tag[table[i].hash]))
			if (plru[table[i].hash][0])
				if (plru[table[i].hash][1])
					if (plru[table[i].hash][3]) table[1].tag[table[i].hash] = table[i].hash;
					else table[1].tag[table[i].hash] = table[i].hash;
				else if (plru[table[i].hash][4]) table[2].tag[table[i].hash] = table[i].hash;
				else table[3].tag[table[i].hash] = table[i].hash;
			else if (plru[table[i].hash][5]) table[4].tag[table[i].hash] = table[i].hash;
			else table[5].tag[table[i].hash] = table[i].hash;
		else if (plru[table[i].hash][6]) table[6].tag[table[i].hash] = table[i].hash;
		else table[7].tag[table[i].hash] = table[i].hash;
	}
}

bool prediction(void){
	return	table[7].match ? (0x1 & (table[7].prediction[table[7].hash] >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[6].match ? (0x1 & (table[6].prediction[table[6].hash] >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[5].match ? (0x1 & (table[5].prediction[table[5].hash] >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[4].match ? (0x1 & (table[4].prediction[table[4].hash] >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[3].match ? (0x1 & (table[3].prediction[table[3].hash] >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[2].match ? (0x1 & (table[2].prediction[table[2].hash] >> (ipow(2, PREDICTOR_SIZE) -1))) :
			table[1].match ? (0x1 & (table[1].prediction[table[1].hash] >> (ipow(2, PREDICTOR_SIZE) -1))) :
							 (0x1 & (table[0].prediction[table[0].hash] >> (ipow(2, PREDICTOR_SIZE) -1))) ;
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
}

void update_path_history(bool taken){
	if(taken) 	(PATH_HIST_MASK & ((path_history << 1) | SET_LSB));
	else 		(PATH_HIST_MASK & (path_history << 1));
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
				fprintf(fh, "%-24s\t", tagname[j]);
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