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
#define PREDICTOR_SIZE	3		// Size in bits.
#define WORD			2		// Used for PC Word Alignment.
#define PCMASK			32		// Used to mask PC bits for use with hash.
#define PLRU_SIZE		(NUM_OF_WAYS - 1)
#define INDEX_SIZE		1024

/*	Global Variables	*/
unsigned int path_history;
unsigned int pc_select;
_table table[NUM_OF_WAYS];
unsigned int plru[INDEX_SIZE][PLRU_SIZE];

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

	/*	Debug	*/	debug(0, "NEWLINE");
}

/************************************
************ Function ***************
*********** Definitions *************
************************************/

void initialize(const branch_record_c* br ){
	unsigned int base_value = (PCMASK / NUM_OF_WAYS);

	pc_select = (PCMASK & (br->instruction_addr >> WORD));

	table[0].path_mask = 0;

	for( int i = 1; i < NUM_OF_WAYS; i++){
		table[i].path_mask = (base_value * i) + base_value;
	}
}

void hash(void){
	for(int i = 0; i < NUM_OF_WAYS; i++ ){
		table[i].hash = ( pc_select ^ (path_history & table[i].path_mask));
	}
}

void tag_compare(void){
	for(int i = 0; i < NUM_OF_WAYS; i++){
		table[i].match = (table[i].hash == table[i].tag) ? 0x1 : 0x0;
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
		if(!(table[i].hash == table[i].tag))
			if (plru[table[i].hash][0])
				if (plru[table[i].hash][1])
					if (plru[table[i].hash][3]) table[1].tag = table[i].hash;
					else table[1].tag = table[i].hash;
				else if (plru[table[i].hash][4]) table[2].tag = table[i].hash;
				else table[3].tag = table[i].hash;
			else if (plru[table[i].hash][5]) table[4].tag = table[i].hash;
			else table[5].tag = table[i].hash;
		else if (plru[table[i].hash][6]) table[6].tag = table[i].hash;
		else table[7].tag = table[i].hash;
	}
}

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

void update_predictors(bool taken){
	for( int i = (NUM_OF_WAYS -1); i >= 0; i--){
		if(	table[i].match ){
			if (table[i].prediction == taken){
				if(table[i].prediction < (ipow(2, PREDICTOR_SIZE) -1)) 
					table[i].prediction++;
			}
			else if(table[i].prediction > 0)
					table[i].prediction--;
			break;
		}
	}
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