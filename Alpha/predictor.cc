/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
 * Modified by: Kamali, Michael, and Ram
 * ECE 586, Winter 2020
 * Portland State University
 * Competition Predictor
*/

#include "predictor.h"

/*Variables- gshare*/
#define BRANCH_HIST_MASK 0x1FF			//10 bits for branch history
#define BAMASK 0x1FF					//10 bits for branch Address
#define PREDICT_SIZE 512				//10 bits for Predictor Table Size
#define SETLSB 0x1
//#define BS_VERBOSE

static unsigned int xor_output = 0;
static unsigned int branch_address = 0;
static unsigned int path_history = 0;
static unsigned int predict_table_g[PREDICT_SIZE] = {0};
static unsigned int predict_table_l[PREDICT_SIZE] = {0};
static unsigned int choice_table[PREDICT_SIZE] = {0};

/*function Prototypes- gshare*/
unsigned int xor_comb(unsigned int a, unsigned int b);
bool get_predict(void);				// returs prediction
void update_global_branch_history(bool);				// updates path history
void update_predict_table(bool);						// updates predictor pointed to by hash
void get_branch_address(const branch_record_c* br);
void update_predict_table_l(bool taken);
void update_predict_table_g(bool taken);
void update_choice_table(bool taken);
void debug(unsigned int val, const char *tag);			// prints values in column format


bool PREDICTOR::get_prediction(const branch_record_c* br  , const op_state_c* os ){
	//Gshare
	//if(!(br->is_conditional)) return 1;

	//Get address branch
	get_branch_address(br);

	/*	Hash the branch history and instruction address	*/
	xor_output = xor_comb(path_history, branch_address);

	/*	Make prediction	*/
	return get_predict();

}


void get_branch_address(const branch_record_c* br)
{
	branch_address = (BAMASK & ( br->instruction_addr >> 2));
	/*	Debug	*/	debug(branch_address, "branch_address_g");
}

unsigned int xor_comb(unsigned int a, unsigned int b)
{
	return  (a ^ b);
}

bool get_predict(void)
{
	/*	Debug	*/ 	debug(xor_output, "xor_output");
	/*	Debug	*/	debug(predict_table_l[xor_output], "predict_table_l[xor]");
	/*	Debug	*/	debug(predict_table_g[xor_output], "predict_table_g[xor]");
	/*	Debug	*/	debug(choice_table[xor_output], "choice_table[xor]");

	if		(0x1 & (choice_table[xor_output] >> 1))
	return	((predict_table_g[xor_output] >> 1) & 0x1) ? true : false;
	else 
	return	((predict_table_l[xor_output] >> 1) & 0x1) ? true : false;
    
}

 /********************************************************************************************************/  

// Update the predictor after a prediction has been made.  This should accept
// the branch record (br) and architectural state (os), as well as a third
// argument (taken) indicating whether or not the branch was taken.
void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken){
	/*	Update tables	*/
	update_predict_table_l(taken);
	update_predict_table_g(taken);
	update_choice_table(taken);

	//Update path history values
	update_global_branch_history(taken);
	
	/*	Debug	*/	debug(0, "NEWLINE");

}

void update_global_branch_history(bool taken)
{
	if(taken) path_history = BRANCH_HIST_MASK & ((path_history << 1) | SETLSB);
	else path_history = BRANCH_HIST_MASK & (path_history << 1);
	/*	Debug	*/	debug(path_history, "path_history");

}

void update_predict_table_l(bool taken)
{
		if(taken)
        {
			if(predict_table_l[xor_output] < 0x3) predict_table_l[xor_output]++;
        }
		else if(predict_table_l[xor_output] > 0x0) predict_table_l[xor_output]--;

		/*	Debug	*/	debug(predict_table_g[xor_output], "Predict_table_g[xor]-U");
}

void update_predict_table_g(bool taken)
{
		if(taken)
        {
			if(predict_table_g[xor_output] < 0x3) predict_table_g[xor_output]++;
        }
		else if(predict_table_g[xor_output] > 0x0) predict_table_g[xor_output]--;

		/*	Debug	*/	debug(predict_table_g[xor_output], "Predict_table_g[xor]-U");
}

void update_choice_table(bool taken)
{
	if (((predict_table_g[xor_output] >> 1) & 0x1) == taken) {					// ?/?
		if (!(((predict_table_l[xor_output] >> 1) & 0x1) == taken)) {				// 1/?
			if (choice_table[xor_output] < 0x3)		// 1/0
				choice_table[xor_output]++;
		}
	}
	else if (((predict_table_l[xor_output] >> 1) & 0x1) == taken) { 				// 0/?
		if (choice_table[xor_output] > 0x0)		// 0/1
			choice_table[xor_output]--;
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