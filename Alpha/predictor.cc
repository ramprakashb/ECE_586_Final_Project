/* Author: Mark Faust;   
 * Description: This file defines the two required functions for the branch predictor.
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

//new changes
 unsigned int local_history_table[(LHTHEIGHT-1):0];
 unsigned int local_prediction_table[(LPTHEIGHT-1):0];
 unsigned int global_prediction_table[(GPTHEIGHT-1):0];
 unsigned int choice_prediction_table[(CPHEIGHT-1):-];
int local_history_table_func(unsigned int);
int local_predictor(unsigned int);
int global_predictor(unsigned int);
int choice_predictor(unsigned int);
bool mux_logic(unsigned int,unsigned int,unsigned int);
unsigned int local_history_value = 0;



 unsigned int local_history_table[(LHTHEIGHT-1)];
 unsigned int local_prediction_table[(LPTHEIGHT-1)];
 unsigned int global_prediction_table[(GPTHEIGHT-1)];
 unsigned int choice_prediction_table[(CPHEIGHT-1)];
 unsigned int path_history;

    bool PREDICTOR::get_prediction(const branch_record_c* br, const op_state_c* os)
        {
		/* replace this code with your own
            bool prediction = false;

			printf("%0x %0x %1d %1d %1d %1d ",br->instruction_addr,
			                                br->branch_target,br->is_indirect,br->is_conditional,
											br->is_call,br->is_return);
											
            if (br->is_conditional)
                prediction = true;
		*/

		unsigned int pc;	
        unsigned int entry_avail;    // update if a entry is available in the local history table
        unsigned int local_predict_val;
        unsigned int global_predict_val;
        unsigned int path_history_val;
        unsigned int choice_predict_val;
        bool prediction;
		//Choice Prediction
		//Local Predictor
		//local history table
        pc = ((br->instruction_addr)>>2) & LHTADDRMASK; //PC[11:2]

        //12 bits of path history:
        path_history_val = path_history & GPTADDRMASK;

        //get the 10 bit value from the local history table ->i/p: PC value o/p:10 bit value indicating the local_history
        //local_history_table() 
        entry_avail=local_history_table_func(pc);//check if the current branch is available in local history table

        //get the local prediction state value from the local predictor table: i/p: 10 bit value calculated above, o/p: state values in the table
        if((entry_avail>=0)&&(entry_avail<=8)){
            local_predict_val = local_predictor(pc);
        }


        //get the path history Path_history[11:0]: o/p: 12 bit path history value in the table
        //get_path_history();
        //use path history variable


		//Global Predictor: i/p: return value from the get_path_history(), o/p: 2 bit global state output.
		global_predict_val = global_predictor(path_history_val);


        //Choice predictor: i/p: return value from the get_path_history(), o/p: choice predicted.
        choice_predict_val = choice_predictor(path_history_val); //choose between global and local predictor

        //mux logic: i/p: output of the local_predictor() function, output of the global_predictor(), output of hte path_history fuction function o/p: branch predicted value 
        prediction = mux_logic(choice_predict_val, global_predict_val, local_predict_val);


        //printf("prediction=%d, br->is_conditional=%d\n",prediction,br->is_conditional);
        return prediction;   // true for taken, false for not taken
        }

    int local_history_table_func(unsigned int pc)
    {
        int i;
        int set;
            if(local_history_table[pc] == 0)
                set = 1;
            else
                set =  0;
    
        if(set)
            return 1;
        else    
            return 0;
    }
    
    int local_predictor(unsigned int pc)
    {
        if((local_prediction_table[pc]<=7)&&(local_prediction_table[pc]>=0)) //Check if the values in the prediction table lies between 000 to 111
        {
            switch(local_prediction_table[pc])
            {
            case 0://if the local prediction table contains 000 (weakly taken)
                return 0;
            case 1://if the local prediction table contains 001
                return 1;
            case 2://if the local prediction table contains 010
                return 2;
            case 3://if the local prediction table contains 011
                return 3;
            case 4://if the local prediction table contains 100
                return 4;
            case 5://if the local prediction table contains 101
                return 5;
            case 6://if the local prediction table contains 110
                return 6;
            case 7://if the local prediction table contains 111 (strongly taken)
                return 7;
            }
        }
        return 8; //indicating value not available in the local predictor table
    }

    int global_predictor(unsigned int path_history_val)
    {
        if((global_prediction_table[path_history_val]<=3)&&(global_prediction_table[path_history_val]>=0)) //Checking if there is a entry in the global prediction table
        {
            switch(global_prediction_table[path_history_val])
            {
            case 0://if the local prediction table contains 000 (weakly taken)
                return 0;
            case 1://if the local prediction table contains 001
                return 1;
            case 2://if the local prediction table contains 010
                return 2;
            case 3://if the local prediction table contains 011
                return 3;
            }
        }
        return 8;
    }



    int choice_predictor(unsigned int path_history_val)
    {
        if((choice_prediction_table[path_history_val]<=4)&&(choice_prediction_table[path_history_val]>=0))
        {
            switch(choice_prediction_table[path_history_val])
            {
            case 0://if the local prediction table contains 000 (weakly taken)
                return 0;
            case 1://if the local prediction table contains 001
                return 0;
            case 2://if the local prediction table contains 010
                return 1;
            case 3://if the local prediction table contains 011
                return 1;
            }
        }
        return 8;
    }

    bool mux_logic(unsigned int choice_predict_val, unsigned int global_predict_val, unsigned int local_predict_val)
    {
        int mux_local_output;
        int mux_global_output;
        bool return_val;
        switch(choice_predict_val)
        {
            case 0:
                    mux_local_output = local_predict_val;
            case 1:
                    mux_global_output = global_predict_val;
        }
        switch(mux_local_output)
        {
            case 0:
                   return_val = false; //return false if branch not taken
            case 1:
                   return_val = false;
            case 2:
                   return_val = true;
            case 3:
                   return_val = true;
        }
        switch(mux_global_output)
        {
            case 0:
                   return_val = false;
            case 1:
                   return_val = false;
            case 2:
                   return_val = false;
            case 3:
                   return_val = false;
            case 4:
                   return_val = true;
            case 5:
                   return_val = true;
            case 6:
                   return_val = true;
            case 7:
                   return_val = true;
        }
        return return_val;
    }



    // Update the predictor after a prediction has been made.  This should accept
    // the branch record (br) and architectural state (os), as well as a third
    // argument (taken) indicating whether or not the branch was taken.
    void PREDICTOR::update_predictor(const branch_record_c* br, const op_state_c* os, bool taken)
        {
         /* replace this code with your own */
			
        unsigned int pc; 
        int temp;
        pc = ((br->instruction_addr)>>2) & LHTADDRMASK;
        path_history = ((taken)>>2) & GPTADDRMASK;                        //12 bits of PathHistory

        
        //update local predictor: i/p: output of the local_history_table() function, 
        //update_local_predictor
        update_local_predict_table(local_history_table[pc]);
    

		// Local Table Update: i/p: pc[11:0] 
        //update_local_history_table();

        update_local_history()
        
        //i/p: output of the get_path_history() function, 
        //update_global_predictor();
        
        //i/p: output of the get_path_history() function.
        //update_choice_predictor

        //update path_history(): i/p: taken
        //update_path_history(taken);/////////////////////
		
		
		
		
        
	int update_local_history_table(int pc)
    {
    }
		
		
	int update_local_predictor(int index)                                            
	if(entry_avail == -1 || (local_predict_value >= -1 && local_predict_value <= 8))                              ///range doubt
	{
    switch(local_prediction_table[index])
    {
            case 0:
            if(taken == 0)
            {
            local_prediction_table[local_history_table] = 0;
            }
            else if(taken == 1)
            {
            local_prediction_table[local_history_table] = 1;
            }

            case 1:
            if(taken == 0)
            {
            local_prediction_table[local_history_table] = 0;
            }
            else if(taken == 1)
            {
            local_prediction_table[local_history_table] = 2;
            }

            case 2:
            if(taken == 0)
            {
            local_prediction_table[local_history_table] = 1;
            }
            else if(taken == 1)
            {
            local_prediction_table[local_history_table] = 3;
            }

            case 3:
            if(taken == 0)
            {
            local_prediction_table[local_history_table] = 2;
            }
            else if(taken == 1)
            {
            local_prediction_table[local_history_table] = 4;
            }

            case 4:
            if(taken == 0)
            {
            local_prediction_table[local_history_table] = 3;
            }
            else if(taken == 1)
            {
            local_prediction_table[local_history_table] = 5;
            }

            case 5:
            if(taken == 0)
            {
            local_prediction_table[local_history_table] = 4;
            }
            else if(taken == 1)
            {
            local_prediction_table[local_history_table] = 6;
            }

            case 6:
            if(taken == 0)
            {
            local_prediction_table[local_history_table] = 4;
            }
            else if(taken == 1)
            {
            local_prediction_table[local_history_table] = 6;
            }

            case 7:
            if(taken == 0)
            {
            local_prediction_table[local_history_table] = 6;
            }
            else if(taken == 1)
            {
            local_prediction_table[local_history_table] = 8;
            }

            case 8:
            if(taken == 0)
            {
            local_prediction_table[local_history_table] = 7;
            }
            else if(taken == 1)
            {
            local_prediction_table[local_history_table] = 8;
            }
            }
			
		}
		}   
        }                                                                                                  
		 
        int update_global_predictor(int path_history)
		{
            switch(global_prediction_table[path_history])
            {
            case 0:
            if(taken == 0)
            {
            global_prediction_table[path_history] = 0;
            }
            else if(taken == 1)
            {
            global_prediction_table[path_history] = 1;
            }

            case 1:
            if(taken == 0)
            {
            global_prediction_table[path_history] = 0;
            }
            else if(taken == 1)
            {
            global_prediction_table[path_history] = 2;
            }

            case 2:
            if(taken == 0)
            {
            global_prediction_table[path_history] = 1;
            }
            else if(taken == 1)
            {
            global_prediction_table[path_history] = 3;
            }

            case 3:
            if(taken == 0)
            {
            global_prediction_table[path_history] = 2;
            }
            else if(taken == 1)
            {
            global_prediction_table[path_history] = 3;
            }

            }
		
		}
		
		int update_choice_predictor(int path_history)
		{
		   switch(global_choice_table[path_history])
            {
            case 0:
            if(taken == 0)
            {
            global_choice_table[path_history] = 0;
            }
            else if(taken == 1)
            {
            global_choice_table[path_history] = 1;
            }

            case 1:
            if(taken == 0)
            {
            global_choice_table[path_history] = 0;
            }
            else if(taken == 1)
            {
            global_choice_table[path_history] = 2;
            }

            case 2:
            if(taken == 0)
            {
            global_choice_table[path_history] = 1;
            }
            else if(taken == 1)
            {
            global_choice_table[path_history] = 3;
            }

            case 3:
            if(taken == 0)
            {
            global_choice_table[path_history] = 2;
            }
            else if(taken == 1)
            {
            global_choice_table[path_history] = 3;
            }

            if(global_choice_table == 1 || 2)                         //return to mux to choose between local and global predictors, make changes in mux
            {
               return 0;
            }
            else if(global_choice_table == 3 || 4 )
            {
                return 1;
            }


            }
		}
		
		int update_path_history(bool taken)                     //update path history(taken);
		{
            temp = path_history << 1
            path_history = temp | taken;                        //shift the path history by left 1 time and add taken
        }

        }
		
        printf("%1d\n",taken);
		
		
        


        

  

	
