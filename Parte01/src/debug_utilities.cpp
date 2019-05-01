/*
 * 	Cache simulation project
 * 	Class UCR IE-521
 * 	Semester: I-2019
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include "../include/debug_utilities.h"

#define CYN "\x1B[36m"
#define RED "\x1B[1;31m"
#define BLUE "\x1B[1;34m"
#define RESET "\x1B[0m"

/* Get enviroment var */
void get_env_var( const char* var_name,
                 int *var_value )
{
    char* pPath = getenv(var_name);
    if (pPath==NULL) {
      *var_value = 0;
	} else {
	     *var_value= atoi(pPath);
	}
}

/* Print way info */
void print_way_info(int idx,
                    int associativity,
                    entry* cache_blocks)
{
  int j = 0;
  printf("%08X\n", idx);
  for (j = 0 ; j< associativity; j++) {
	printf(CYN "INFO: " RESET "Way #%d: tag: %08X --- valid: ", j, cache_blocks[j].tag);

    if(cache_blocks[j].valid){
        printf(RED);
    }
    printf("%d", cache_blocks[j].valid);
    if(cache_blocks[j].valid){
        printf(RESET);
    }
    printf(" rp_value: %d --- dirty: ", cache_blocks[j].rp_value);
    if(cache_blocks[j].dirty){
        printf(RED);
    }
    printf("%d\n", cache_blocks[j].dirty);
    if(cache_blocks[j].dirty){
        printf(RESET);
    }
  }
  printf("\n");

}
