/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
 * Modified by: Kamali, Michael, and Ram
 * ECE 586, Winter 2020
 * Portland State University
*/

#include "Alpha/predictor.h"
#include "Alpha/predictor.cc"
#include "Alpha/tread.h"
#include <cstdio>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>

branch_record_c br;


int main(int argc, char *argv[])
	
	
	bool predicted_taken;
	bool actual_taken;

	/*	Open Trace File	*/
	FILE * input_file = NULL;
	input_file = fileopen(argv[1], "r");

	/* Process	*/
	while(!feof(input_file)){ // Check for EOF.

		/*	Read From File	*/
		if(	0 == fscanf(input_file, "%u %u\n", br->instruction_addr, actual_taken)) printf("FAILED.");

		bool predicted_taken = predictor.get_prediction(&br, cbptr.osptr);
		
		predictor.update_predictor(&br, cbptr.osptr, actual_taken);

	}

	return 0;
}