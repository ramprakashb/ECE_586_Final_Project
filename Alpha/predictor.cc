/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
 * Modified by: Kamali, Michael, and Ram
 * ECE 586, Winter 2020
 * Portland State University
 * Competition Predictor
*/

#include "predictor.h"

/*Variables- gshare*/
#define BHMASK 0xFFF			//10 bits for branch history
#define BAMASK 0xFFF			//10 bits for branch Address
#define PREDICT_SIZE 4*1024		//10 bits for Predictor Table Size
#define SETLSB 0x1
#define PATHHISTVAL 0xFFF
//#define BS_VERBOSE

static unsigned int xor_output = 0;
static unsigned int branch_address = 0;
static unsigned int path_history = 0;
static unsigned int predict_table_g[PREDICT_SIZE] = {0};

/*function Prototypes- gshare*/
unsigned int xor_comb(unsigned int a, unsigned int b);
bool get_predict(void);				// returs prediction
void update_global_branch_history(bool);				// updates path history
void update_predict_table(bool);						// updates predictor pointed to by hash
void get_branch_address(const branch_record_c* br);
void debug(unsigned int val, const char *tag);			// prints values in column format



bool PREDICTOR::get_prediction(const branch_record_c* br  , const op_state_c* os ){
	//Gshare
	//Get address branch
	get_branch_address(br);

	//xor
	xor_output = xor_comb(path_history, branch_address);

	//Get Prediction table values
	return get_predict();

}


void get_branch_address(const branch_record_c* br)
{
	branch_address = (BAMASK & ( br->instruction_addr >> 2));
	/*	Debug	*/	debug(branch_address, "branch_address_g");
}

unsigned int xor_comb(unsigned int a, unsigned int b)
{
	return  (BHMASK & (a ^ b));
}
bool get_predict(void)
{
	/*	Debug	*/ 	debug(xor_output, "xor_output");
	/*	Debug	*/	debug(predict_table_g[xor_output], "predict_table_g[xor]");

    if ((predict_table_g[xor_output] >> 1) & 0x1)	// Return value based on MSb.
        return true;
	else
        return false;
    
}
    
// Update the predictor after a prediction has been made.  This should accept
// the branch record (br) and architectural state (os), as well as a third
// argument (taken) indicating whether or not the branch was taken.
void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken){

	//Update path history values
	update_global_branch_history(taken);

	//update predict table
	update_predict_table(taken);

	/*	Debug	*/	debug(0, "NEWLINE");

}

void update_global_branch_history(bool taken)
{
	if(taken)
		path_history = PATHHISTVAL & ((path_history << 1) | SETLSB);
	else
		path_history = PATHHISTVAL & (path_history << 1);
	
	/*	Debug	*/	debug(path_history, "path_history");

}

void update_predict_table(bool taken)
{
		if(taken)
        {
			if(predict_table_g[xor_output] < 0x3)
			 	predict_table_g[xor_output]++;
        }
		else if(predict_table_g[xor_output] > 0x0)
        {
				predict_table_g[xor_output]--;
        }

		/*	Debug	*/	debug(predict_table_g[xor_output], "Predict_table_g[xor]-U");
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