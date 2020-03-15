/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
 * Modified by: Kamali, Michael, and Ram
 * ECE 586, Winter 2020
 * Portland State University
 * Competition Predictor
*/

#include "predictor.h"

/*Variables- gshare*/

#define BHMASK 0x3FF			//10 bits for branch history
#define BAMASK 0x3FF			//10 bits for branch Address
#define XORMASK 0x3FF			//10 bits for XOR Address
#define PREDICTOR_SIZE 2 		// 2-bit predictor counter
#define PREDICT_TABLE_SIZE 1024	//Prediction table of size 1024
#define TAGMASK 0xFFFFF			//20 bits for TAG Address
#define SETLSB 0x1
#define PATHHISTVAL 0xFFF

static unsigned int branch_history_g = 0;
static unsigned int xor_output_g = 0;
static unsigned int path_history = 0;
static unsigned int branch_address_g = 0;
static unsigned int predict_output = 0;
static unsigned int predict_tag_g = 0;
static unsigned int predict_set_g = 0;
/*Data Structures- gshare*/
 static unsigned int predict_table_g[PREDICT_SIZE];
 static unsigned int predict_table_set_g[PREDICT_SIZE];
 static unsigned int predict_table_tag_g[PREDICT_SIZE];



/////////////////////////////////Gshare Predictor//////////////////////////////////
/*function Prototypes- gshare*/
void get_global_branch_history_g();
void get_branch_address(const branch_record_c*);
void get_xor_operation();
bool get_predict(const branch_record_c*);
void update_global_branch_history(bool);
void update_predict_table(bool);
void debug(unsigned int val, const char *tag);




bool PREDICTOR::get_prediction(const branch_record_c* br  , const op_state_c* os ){
	//Gshare

	//Get Global Branch History values
	get_global_branch_history_g();

	//Get Branch Address
	get_branch_address(br);


	//XOR Operation of Global and Branch
	get_xor_operation();

	//Get Prediction table values
	return get_predict(br);

}


void get_global_branch_history_g(void)
{
	branch_history_g = path_history & BHMASK;
}

void get_branch_address(const branch_record_c* br)
{
	branch_address_g = (BAMASK & (br->instruction_addr >> 2));
}

void get_xor_operation(void)
{
	xor_output_g = branch_history_g ^ branch_address_g;
}
bool get_predict(const branch_record_c* br)
{
    xor_output_g = xor_output_g & 0x3FF;
	predict_tag_g = predict_table_tag_g[xor_output_g];
    predict_tag_g = predict_tag_g & TAGMASK; 
	predict_set_g = predict_table_set_g[predict_tag_g];
	predict_output = (0x1 & (predict_table_g[predict_set_g] >> 1));
    if (predict_output)
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

}

void update_global_branch_history(bool taken)
{
	if(taken)
		path_history = PATHHISTVAL & ((path_history << 1) | SETLSB);
	else
		path_history = PATHHISTVAL & (path_history << 1);
}

void update_predict_table(bool taken)
{
		if(predict_output == taken)
        {
			if(predict_set_g < 0x3)
			 	predict_table_set_g[predict_tag_g]++;
        }
		else if(predict_set_g > 0x1)
        {
				predict_table_set_g[predict_tag_g]--;
        }
}

void debug(unsigned int val, const char *tag){
	#ifdef	BS_VERBOSE
	FILE * fh = NULL;
	fh = fopen("log.txt", "a");
	
	char binary[13] = {0};
	static char tagname[20][256] = {'\0'};
	static char tagval[20][13] = {'\0'};
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
						fprintf(fh, "%-12s\t", tagname[j]);
					}
					fprintf(fh, "\n");
					header = 1;
				}
				for(int j = 0; j < i; j++){
						fprintf(fh, "%-12s\t", tagval[j]);
				}
				fprintf(fh, "\n");
				i = 0;
	}

	fclose(fh);
	#endif
}