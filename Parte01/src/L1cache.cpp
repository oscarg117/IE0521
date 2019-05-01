/*
 *  Cache simulation project
 *  Class UCR IE-521
 *  Semester: I-2019
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include "../include/debug_utilities.h"
#include "../include/L1cache.h"

#define KB 1024
#define ADDRSIZE 32
using namespace std;
int field_size_get( int cachesize_kb,
                    int associativity,
                    int blocksize_bytes,
                    int *tag_size,
                    int *idx_size,
                    int *offset_size)
{

  int blocks = (cachesize_kb * KB)/blocksize_bytes;

  *idx_size = int(log2(blocks / associativity));
  *offset_size = int(log2(blocksize_bytes));
  *tag_size = ADDRSIZE - *idx_size - *offset_size;

  return OK;
}

void address_tag_idx_get(long address,
                        int tag_size,
                        int idx_size,
                        int offset_size,
                        int *idx,
                        int *tag)
{

  unsigned int tmp = address << tag_size;
  *idx = tmp >> (tag_size + offset_size);
  *tag = address >> (idx_size + offset_size);

}

int srrip_replacement_policy(int idx,
                             int tag,
                             int associativity,
                             bool loadstore,
                             entry* cache_blocks,
                             operation_result* result,
                             bool debug)
{
    int i, j;
    int srrip_value; // 2^m
    if(associativity <= 2){
        srrip_value = 1<<1; // 2^1
    } else {
        srrip_value = 1<<2; // 2^2
    }

    // Searching tag
    for(i = 0; i < associativity; i++){ //Iterates over the entry
        if(cache_blocks[i].valid){ //Checks if data is valid
            if(cache_blocks[i].tag == tag){ //Checks tag
                // There is a HIT
                // Sets the result as a HIT
                if(loadstore){ //Checks operation type
                    result->miss_hit = HIT_STORE;
                    result->dirty_eviction = false;
                } else {
                    result->miss_hit = HIT_LOAD;
                    result->dirty_eviction = false;
                }
                // Update RP Value
                cache_blocks[i].rp_value = 0;
                return OK; // Exits RP Function
            }
        }
    }
    //If you're here, well, there is a MISS
    if(loadstore){ //Checks operation type
        result->miss_hit = MISS_STORE;
        result->dirty_eviction = false;
    } else {
        result->miss_hit = MISS_LOAD;
        result->dirty_eviction = false;
    }
    // Insert block from main memory
    for(i = 0; i < srrip_value; i++){ // For loop  N1
        // Searching RP Value = 2^m-1
        for(j = 0; j < associativity; j++){ // For loop N2
           if (cache_blocks[j].rp_value == srrip_value - 1) {
               i = srrip_value; // temp var to exit For Loop N1
               cache_blocks[j].tag = tag;
               cache_blocks[j].rp_value = srrip_value - 2;
               cache_blocks[j].valid = true;
               break; // Exits For loop N2
           }
        }
        if(i == srrip_value){ break; } // Exits For loop N1
        // Increases RP Value
        for(j = 0; j < associativity; j++){
            cache_blocks[i].rp_value++;
        }
    }

    return OK;
 }




int lru_replacement_policy (int idx,
                             int tag,
                             int associativity,
                             bool loadstore,
                             entry* cache_blocks,
                             operation_result* result,
                             bool debug)
{
    for (int i = 0; i < associativity; i++){
        cache_blocks[i].valid = true;
        cache_blocks[i].dirty = false;
        cache_blocks[i].tag = tag;
        cache_blocks[i].rp_value = i;
    }

    if(loadstore){
        result->miss_hit = MISS_STORE;
    } else {
        result->miss_hit = MISS_LOAD;
    }
    result->dirty_eviction = false;

   return OK;
}
