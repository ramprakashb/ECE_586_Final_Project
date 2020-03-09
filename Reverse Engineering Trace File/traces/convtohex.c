#include<stdio.h>
#include<inttypes.h>

typedef uint16_t KEY_TYPE;
/* first byte */															// First byte
static const KEY_TYPE TWO_BYTE_KEY          = ((0x1) << 0 );		// 0000_0001
static const KEY_TYPE MASK_DST_VAL          = ((0x3) << 1 );		// 0000_0110
static const KEY_TYPE TYPE0_DST_VAL         =  ((0) << 1 );			// 0000_0000
static const KEY_TYPE TYPE1_DST_VAL         =  ((1) << 1 );			// 0000_0010
static const KEY_TYPE TYPE2_DST_VAL         =  ((2) << 1 );			// 0000_0100
static const KEY_TYPE READ_DST_VAL          =  ((3) << 1 );			// 0000_0110
static const KEY_TYPE MASK_VADDR1           = ((0x3) << 3 );		// 0001_1000
static const KEY_TYPE TYPE0_VADDR1          =  ((0) << 3 );			// 0000_0000
static const KEY_TYPE TYPE1_VADDR1          =  ((1) << 3 );			// 0000_1000
static const KEY_TYPE TYPE2_VADDR1          =  ((2) << 3 );			// 0001_0000
static const KEY_TYPE READ_VADDR1           =  ((3) << 3 );			// 0001_1000
static const KEY_TYPE READ_SRC2_VAL         = ((0x1) << 5 );		// 0010_0000
static const KEY_TYPE FLIP_TAKEN            = ((0x1) << 6 );		// 0100_0000
static const KEY_TYPE READ_STATIC_INFO      = ((0x1) << 7 );		// 1000_0000

/* second byte */															// Second byte
static const KEY_TYPE READ_INSTRUCTION_ADDR = ((0x1) << 8 );		// 0000_0001
static const KEY_TYPE MASK_BRANCH_TARGET    = ((0x3) << 9 );		// 0000_0110
static const KEY_TYPE TYPE0_BRANCH_TARGET   =  ((0) << 9 ); 		// 0000_0000
static const KEY_TYPE TYPE1_BRANCH_TARGET   =  ((1) << 9 ); 		// 0000_0010
static const KEY_TYPE TYPE2_BRANCH_TARGET   =  ((2) << 9 ); 		// 0000_0100
static const KEY_TYPE READ_BRANCH_TARGET    =  ((3) << 9 ); 		// 0000_0110
static const KEY_TYPE READ_SRC1_VAL         = ((0x1) << 11);		// 0000_1000
static const KEY_TYPE READ_VADDR2           = ((0x1) << 12);		// 0001_0000
static const KEY_TYPE UNUSED_KEY_BIT1       = ((0x1) << 13);		// 0010_0000
static const KEY_TYPE UNUSED_KEY_BIT2       = ((0x1) << 14);		// 0100_0000
static const KEY_TYPE UNUSED_KEY_BIT3       = ((0x1) << 15);		// 1000_0000


int main(int argc, char *argv[]){
	
	FILE *filep = NULL;

	if( (filep = fopen(argv[1], "r")) == NULL ) return 0;
	
	int getc = 0;
	int count = 0;
	KEY_TYPE first_key = 0;
	KEY_TYPE second_key = 0;
	KEY_TYPE whole_key = 0;
	
	while(!feof(filep)){
	
		if(count != 4){
			getc = fgetc(filep);
			if(count == 0) first_key = getc;
			if(count == 1){
				second_key = getc;
				whole_key = (second_key << 8) & first_key;
			printf("%02X", (unsigned char) getc);
			count++;
			}
		}
		else{
			printf("\t");
			for(int i = 7; i >= 0; i--){
				
				printf("%1d", ((first_key >> i) & 1));
				
			}
			printf("\t");
			for(int i = 7; i >= 0; i--){
				
				printf("%1d", ((second_key >> i) & 1));
				
			}
		
			printf("\t");

			if( TWO_BYTE_KEY== whole_key)
				printf("TWO_BYTE_KEY ");
				
			if( MASK_DST_VAL== whole_key)
				printf("MASK_DST_VAL ");
				
			if( TYPE0_DST_VAL== whole_key)
				printf("TYPE0_DST_VAL ");
							
			if( TYPE1_DST_VAL== whole_key)
				printf("TYPE1_DST_VAL ");
											
			if( TYPE2_DST_VAL== whole_key)
				printf("TYPE2_DST_VAL ");
				 
			if( READ_DST_VAL== whole_key)
				printf("READ_DST_VAL ");
				
			if( MASK_VADDR1== whole_key)
				printf("MASK_VADDR1 ");
								
			if( TYPE0_VADDR1== whole_key)   
				printf("TYPE0_VADDR1 ");
								
			if( TYPE1_VADDR1== whole_key)  
				printf("TYPE1_VADDR1 ");
				
			if( TYPE2_VADDR1== whole_key)    
				printf("TYPE2_VADDR1 ");
				
			if( READ_VADDR1== whole_key)   
				printf("READ_VADDR1 ");
				
			if( READ_SRC2_VAL== whole_key)   
				printf("READ_SRC2_VAL ");
				
			if( FLIP_TAKEN== whole_key)     
				printf("FLIP_TAKEN ");
				
			if( READ_STATIC_INFO== whole_key)
				printf("READ_STATIC_INFO ");
				
			if( READ_INSTRUCTION_ADDR== whole_key)
				printf("READ_INSTRUCTION_ADDR ");
				
			if( MASK_BRANCH_TARGET== whole_key)   
				printf("MASK_BRANCH_TARGET ");
				
			if( TYPE0_BRANCH_TARGET== whole_key)  
				printf("TYPE0_BRANCH_TARGET ");
				
			if( TYPE1_BRANCH_TARGET== whole_key)  
				printf("TYPE1_BRANCH_TARGET ");
				
			if( TYPE2_BRANCH_TARGET== whole_key)  
				printf("TYPE2_BRANCH_TARGET ");
				
			if( READ_BRANCH_TARGET== whole_key)  
				printf("READ_BRANCH_TARGET ");
								
			if( READ_SRC1_VAL== whole_key)
				printf("READ_SRC1_VAL ");
				
			if( READ_VADDR2== whole_key)          
				printf("READ_VADDR2 ");
				
			if( UNUSED_KEY_BIT1== whole_key)     
				printf("UNUSED_KEY_BIT1 ");
				
			if( UNUSED_KEY_BIT2== whole_key)      
				printf("UNUSED_KEY_BIT2 ");
				
			if( UNUSED_KEY_BIT3== whole_key)      
				printf("UNUSED_KEY_BIT3 ");;
					
			printf("\n");
			count = 0;
		}
		
	}

	fclose(filep);
	
	
}

