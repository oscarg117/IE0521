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

int srrip_replacement_policy (int idx,
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


int lru_replacement_policy (int idx,
                             int tag,
                             int associativity,
                             bool loadstore,
                             entry* cache_blocks,
                             operation_result* result,
                             bool debug)
{
   return ERROR;
}
