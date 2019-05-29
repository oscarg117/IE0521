#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <math.h>
#include "../include/L1cache.h"
#include "../include/debug_utilities.h"

using namespace std;


/* Helper funtions */
void print_usage ()
{
  cerr << "Uso: " << "cache" << " -t <Tamaño Cache (kB)> -l <Tamaño Bloque (B)> -a <n_way> -opt <OPT>" << endl;
  exit (0);
}

/* enum to iostream*/
std::ostream& operator<<(std::ostream& out, const replacement_policy value){
    const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(LRU);
        PROCESS_VAL(NRU);
        PROCESS_VAL(RRIP);
        PROCESS_VAL(RANDOM);
    }
#undef PROCESS_VAL

    return out << s;
}

std::ostream& operator<<(std::ostream& out, const cache_optimization value){
    const char* s = 0;
#define PROCESS_VAL(p) case(p): s = #p; break;
    switch(value){
        PROCESS_VAL(VC);
        PROCESS_VAL(L2);
        PROCESS_VAL(NONE);
    }
#undef PROCESS_VAL

    return out << s;
}

int main(int argc, char * argv []) {

  /* Cache variables */
  /* From command line */
  int tam_cache_kb = 0;
  int tam_bloque_b = 0;
  int n_way = 0;
  cache_optimization opt = RANDOM;
  /* From functions */
  int tam_tag = 0;
  int tam_idx = 0;
  int tam_offset = 0;
  /* From simulation */
  long long cpu_time = 0;
  int amat = 0;
  double overall_miss_rate = 0;
  double read_miss_rate = 0;
  int dirty_evictions = 0;
  int load_misses = 0;
  int store_misses = 0;
  int total_misses = 0;
  int load_hits = 0;
  int store_hits = 0;
  int total_hits = 0;

  string line_str =  "--------------------------------------------";
  string line_str0 = "____________________________________________";
  /***********************************************/

  /* Parse argruments */

  if (argc < 7) {
    print_usage();
  }

  for (size_t i = 0; i < argc; i++) {
    string arg = argv[i];
    if (arg == "-t") {
      if(i + 1 < argc){
        tam_cache_kb = stoi(argv[i+1]);
      } else {
        print_usage();
      }
    }
    if (arg == "-l") {
      if(i + 1 < argc){
        tam_bloque_b = stoi(argv[i+1]);
      } else {
        print_usage();
      }
    }
    if (arg == "-a") {
      if(i + 1 < argc){
        n_way = stoi(argv[i+1]);
      } else {
        print_usage();
      }
    }
    if (arg == "-opt") {
      if(i + 1 < argc){
        string argOPT = argv[i+1];
        //Assigns cache optimization
        if (argOPT == "VC" || argOPT == "vc") {
          opt = VC;
        } else if (argOPT == "L2" || argOPT == "l2") {
          opt = L2;
        } else {  //Default: NONE
          opt = NONE;
        }
      } else {
        print_usage();
      }
    }
  }
  // Checks if all parameters are assigned
  if(tam_cache_kb <= 0 || tam_bloque_b <= 0 || n_way <= 0){
    print_usage();
  }
  // Getting tag, index and offset sizes
  field_size_get( tam_cache_kb, n_way, tam_bloque_b,
                  &tam_tag, &tam_idx, &tam_offset);


  //Cache instance
  int n_blocks = 1<<tam_idx;
  entry cache[n_blocks][n_way];
  for (int i = 0; i < n_blocks; i++){
      for (int j = 0; j < n_way; j++){
          cache[i][j].valid = false;
          cache[i][j].dirty = false;
          cache[i][j].rp_value = n_way - 1;
      }
  }

  /* Get trace's lines and start your simulation */
  string ctrl = "#";
  bool op;
  long long address;
  int ic;
  int index, tag;
  operation_result op_result;

  //Start simulation
  while (ctrl == "#") {
    ctrl = "N";
    cin >> ctrl;
    cin >> op;
    cin >> hex >> address;
    cin >> dec >> ic;

    //Gets tag and index  from address
    address_tag_idx_get(address, tam_tag, tam_idx, tam_offset,
                        &index, &tag);

    //Keep track of CPU time
    cpu_time = cpu_time + ic;


    //Replacement Policy
    if(rp == RRIP){
        srrip_replacement_policy(index, tag,  n_way, op,
                                 cache[index], &op_result);
    } else {
        lru_replacement_policy(  index, tag,  n_way, op,
                                 cache[index], &op_result);
    }

    switch (op_result.miss_hit)
    {
    case MISS_LOAD:
        load_misses++;
        cpu_time+=20;
        break;
    case MISS_STORE:
        store_misses++;
        break;
    case HIT_LOAD:
        load_hits++;
        break;
    case HIT_STORE:
        store_hits++;
        break;
    default:
        break;
    }

    if(op_result.dirty_eviction){
        dirty_evictions++;
    }

    //if(cpu_time > 1000){break;}
  }//End while

  //Get cache simulation stats
  total_misses = load_misses + store_misses;
  total_hits = load_hits + store_hits;
  overall_miss_rate = double(total_misses)/double(total_misses + total_hits);
  read_miss_rate = double(load_misses)/double(load_misses + load_hits);
  amat = 1.0 + overall_miss_rate * 20.0;


  /* Print cache configuration */

  cout << endl;
  cout << "Replacement Policy:\t\t" << rp << endl;
/*  cout << "Index size:\t" << tam_idx << endl;
  cout << "Offset size:\t" << tam_offset << endl;
  cout << "Tag size:\t" << tam_tag << endl;
*/  cout << endl;

  if(opt == L2){
    /* Print Statistics */
    cout << line_str << endl;
    cout << "Cache parameters:" << endl;
    cout << line_str << endl;
    cout << "L1 Cache Size (KB):\t\t" << tam_cache_kb << endl;
    cout << "L2 Cache Size (KB):\t\t" << tam_cache_kb << endl;
    cout << "Cache L1 Associativity:\t\t" << n_way << endl;
    cout << "Cache L2 Associativity:\t\t" << n_way << endl;
    cout << "Cache Block Size (bytes):\t" << tam_bloque_b << endl;
    cout << line_str << endl;
    cout << "Simulation results:" << endl;
    cout << line_str << endl;
    cout << "Overall miss rate:\t\t" << overall_miss_rate << endl;
    cout << "L1 miss rate:\t\t\t" << read_miss_rate << endl;
    cout << "L2 miss rate:\t\t\t" << read_miss_rate << endl;
    cout << "Global miss rate:\t\t" << read_miss_rate << endl;
    cout << "Misses (L1):\t\t\t" << load_misses << endl;
    cout << "Hits (L1):\t\t\t" << load_misses << endl;
    cout << "Misses (L2):\t\t\t" << load_misses << endl;
    cout << "Hits (L1):\t\t\t" << load_misses << endl;
    cout << "Dirty Evictions (L2):\t\t" << dirty_evictions << endl;
    cout << line_str << endl << endl;
  } else if (opt == VC){
    cout << line_str << endl;
    cout << "Cache parameters:" << endl;
    cout << line_str << endl;
    cout << "L1 Cache Size (KB):\t\t" << tam_cache_kb << endl;
    cout << "Cache L1 Associativity:\t\t" << n_way << endl;
    cout << "Cache Block Size (bytes):\t" << tam_bloque_b << endl;
    cout << line_str << endl;
    cout << "Simulation results:" << endl;
    cout << line_str << endl;
    cout << "Miss rate (L1 + VC):\t\t" << cpu_time << endl;
    cout << "Misses (L1 + VC):\t\t" << amat << endl;
    cout << "Hits (L1 + VC):\t\t\t" << overall_miss_rate << endl;
    cout << "Victim cache hits:\t\t" << read_miss_rate << endl;
    cout << "Dirty Evictions:\t\t" << dirty_evictions << endl;
    cout << line_str << endl << endl;
  } else if (opt == NONE){
    cout << line_str << endl;
    cout << "Cache parameters:" << endl;
    cout << line_str << endl;
    cout << "L1 Cache Size (KB):\t\t" << tam_cache_kb << endl;
    cout << "Cache L1 Associativity:\t\t" << n_way << endl;
    cout << "Cache Block Size (bytes):\t" << tam_bloque_b << endl;
    cout << line_str << endl;
    cout << "Simulation results:" << endl;
    cout << line_str << endl;
    cout << "Miss rate (L1):\t\t\t" << cpu_time << endl;
    cout << "Misses (L1):\t\t\t" << amat << endl;
    cout << "Hits (L1):\t\t\t" << overall_miss_rate << endl;
    cout << "Victim cache hits:\t\t" << read_miss_rate << endl;
    cout << "Dirty Evictions:\t\t" << dirty_evictions << endl;
    cout << line_str << endl << endl;
  } else {
    cout << "Nunca debería llegar aquí." << endl << endl;
  }

return 0;
}
