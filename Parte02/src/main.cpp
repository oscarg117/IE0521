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
  int tam_cache_kb_L1 = 0;
  int n_way_L1 = 0;
  int tam_cache_kb_L2 = 0;
  int n_way_L2 = 0;
  int tam_cache_kb_VC = 0;
  int n_way_VC = 0;
  int tam_bloque_b = 0;

  cache_optimization opt = NONE;
  /* From functions */
  int tam_tag_L1 = 0;
  int tam_idx_L1 = 0;
  int tam_offset_L1 = 0;
  int tam_tag_L2 = 0;
  int tam_idx_L2 = 0;
  int tam_offset_L2 = 0;
  int tam_tag_VC = 0;
  int tam_idx_VC = 0;
  int tam_offset_VC = 0;
  /* From simulation */
  double miss_rate_global = 0;
  double miss_rate_L1 = 0;
  int misses_L1 = 0;
  int hits_L1 = 0;
  double miss_rate_L2 = 0;
  int misses_L2 = 0;
  int hits_L2 = 0;

  double miss_rate_L1vc = 0;
  int misses_L1vc = 0;
  int hits_L1vc = 0;
  int hits_vc = 0;

  int dirty_evictions = 0;

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
        tam_cache_kb_L1 = stoi(argv[i+1]);
        tam_cache_kb_L2 = 4 * tam_cache_kb_L1;
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
        n_way_L1 = stoi(argv[i+1]);
        n_way_L2 = 2 * n_way_L1;
        n_way_VC = n_way_L1;
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
  if(tam_cache_kb_L1 <= 0 || tam_bloque_b <= 0 || n_way_L1 <= 0){
    print_usage();
  }

  // Getting L1 tag, index and offset sizes
  field_size_get( tam_cache_kb_L1, n_way_L1, tam_bloque_b,
                  &tam_tag_L1, &tam_idx_L1, &tam_offset_L1);

  // Getting L2 tag, index and offset sizes
  field_size_get( tam_cache_kb_L2, n_way_L2, tam_bloque_b,
                  &tam_tag_L2, &tam_idx_L2, &tam_offset_L2);



  tam_cache_kb_VC = 16*tam_bloque_b*n_way_VC;
  // Getting VC tag, index adn offset sizes
  field_size_get_VC(n_way_VC, tam_bloque_b,
                  &tam_tag_VC, &tam_idx_VC, &tam_offset_VC);

  //Cache instances
  //L1
  int n_blocks = 1<<tam_idx_L1;
  entry cache_L1[n_blocks][n_way_L1];
  for (int i = 0; i < n_blocks; i++){
      for (int j = 0; j < n_way_L1; j++){
          cache_L1[i][j].valid = false;
          cache_L1[i][j].dirty = false;
          cache_L1[i][j].rp_value = n_way_L1 - 1;
      }
  }
  //L2
  n_blocks = 1<<tam_idx_L2;
  entry cache_L2[n_blocks][n_way_L2];
  for (int i = 0; i < n_blocks; i++){
      for (int j = 0; j < n_way_L2; j++){
          cache_L2[i][j].valid = false;
          cache_L2[i][j].dirty = false;
          cache_L2[i][j].rp_value = n_way_L2 - 1;
      }
  }

  n_blocks = 1<<tam_idx_VC;
  entry cache_VC[n_blocks][n_way_VC];
  for (int i = 0; i < n_blocks; i++){
      for (int j = 0; j < n_way_VC; j++){
          cache_VC[i][j].valid = false;
          cache_VC[i][j].dirty = false;
          cache_VC[i][j].rp_value = n_way_VC - 1;
      }
  }

  /* Get trace's lines and start your simulation */
  string ctrl = "#";
  bool op;
  int ic = 0;
  long long address;
  int index_L1, tag_L1, index_L2, tag_L2, index_VC, tag_VC;
  operation_result op_result_L1, op_result_L2, op_result_VC;

cout<<index_L1<<"-> index_L1"<<endl;
cout<<index_VC<<"-> index_VC"<<endl;
cout<<index_L2<<"-> index_L2"<<endl;

  /* Print cache configuration */

  cout << endl;
  cout << "Optimization:\t\t\t" << opt << endl;

  cout << endl;

  if(opt == L2){
    // L1 + L2
    //Start simulation
    while (ctrl == "#") {
      ctrl = "N";
      cin >> ctrl;
      cin >> op;
      cin >> hex >> address;
      cin >> dec >> ic;

      //Gets tag and index from address
      address_tag_idx_get(address, tam_tag_L1, tam_idx_L1, tam_offset_L1,
                          &index_L1, &tag_L1);

      //Se busca dato en L1
      lru_replacement_policy(  index_L1, tag_L1,  n_way_L1, op,
                                   cache_L1[index_L1], &op_result_L1);

      if(op_result_L1.miss_hit == HIT_STORE || op_result_L1.miss_hit == HIT_LOAD){
        // L1 Hit
        hits_L1++;
      } else {
        // L1 Miss
        misses_L1++;

        //Gets tag and index from address
        address_tag_idx_get(address, tam_tag_L2, tam_idx_L2, tam_offset_L2,
                            &index_L2, &tag_L2);

        //Se busca dato en L2
        lru_replacement_policy(  index_L2, tag_L2,  n_way_L2, op,
                                     cache_L2[index_L2], &op_result_L2);

        if(op_result_L2.miss_hit == HIT_STORE || op_result_L2.miss_hit == HIT_LOAD){
          // L2 Hit
          hits_L2++;

        } else {
          // L2 Miss
          misses_L2++;

        }

        if(op_result_L2.dirty_eviction){
            dirty_evictions++;
        }
      }

      //if(cpu_time > 1000){break;}
    }//End while

    //Get cache simulation stats
    miss_rate_L1 = double(misses_L1) / double(misses_L1 + hits_L1);
    miss_rate_L2 = double(misses_L2) / double(misses_L2 + hits_L2);
    miss_rate_global = miss_rate_L1 * miss_rate_L2;


    /* Print Statistics */
    cout << line_str << endl;
    cout << "Cache parameters:" << endl;
    cout << line_str << endl;
    cout << "L1 Cache Size (KB):\t\t" << tam_cache_kb_L1 << endl;
    cout << "L2 Cache Size (KB):\t\t" << tam_cache_kb_L2 << endl;
    cout << "Cache L1 Associativity:\t\t" << n_way_L1 << endl;
    cout << "Cache L2 Associativity:\t\t" << n_way_L2 << endl;
    cout << "Cache Block Size (bytes):\t" << tam_bloque_b << endl;
    cout << line_str << endl;
    cout << "Simulation results:" << endl;
    cout << line_str << endl;
    cout << "Global miss rate:\t\t" << miss_rate_global << endl;
    cout << "L1 miss rate:\t\t\t" << miss_rate_L1 << endl;
    cout << "L2 miss rate:\t\t\t" << miss_rate_L2 << endl;
    cout << "Misses (L1):\t\t\t" << misses_L1 << endl;
    cout << "Hits (L1):\t\t\t" << hits_L1 << endl;
    cout << "Misses (L2):\t\t\t" << misses_L2 << endl;
    cout << "Hits (L1):\t\t\t" << hits_L2 << endl;
    cout << "Dirty Evictions (L2):\t\t" << dirty_evictions << endl;
    cout << line_str << endl << endl;

  } else if (opt == VC){
    // L1 + Victim Cache
    //Start simulation

    while (ctrl == "#") {
      ctrl = "N";
      cin >> ctrl;
      cin >> op;
      cin >> hex >> address;
      cin >> dec >> ic;

      //Gets tag and index from address
      address_tag_idx_get(address, tam_tag_L1, tam_idx_L1, tam_offset_L1,
                          &index_L1, &tag_L1);

//cout<<index_L1<<"-> index_L1"<<endl;
      //Se busca dato en L1
      lru_replacement_policy(  index_L1, tag_L1,  n_way_L1, op,
                                   cache_L1[index_L1], &op_result_L1);

      if(op_result_L1.miss_hit == HIT_STORE || op_result_L1.miss_hit == HIT_LOAD){
        // L1 Hit
        hits_L1++;
      } else {
        // L1 Miss
        misses_L1++;


        //Gets tag and index from address
        address_tag_idx_get(address, tam_tag_VC, tam_idx_VC, tam_offset_VC,
                            &index_VC, &tag_VC);


        //Se busca dato en VC
        lru_replacement_policy(16,tag_VC,n_way_VC, op,
                                     cache_VC[16], &op_result_VC);

        if(op_result_VC.miss_hit == HIT_STORE || op_result_VC.miss_hit == HIT_LOAD){
          // VC Hit
          hits_vc++;

        } else {
          // VC Miss
          misses_L1vc++;

        }

        if(op_result_VC.dirty_eviction){
            dirty_evictions++;
        }
      }

      //if(cpu_time > 1000){break;}
    }//End while

    //Get cache simulation stats
    miss_rate_L1 = double(misses_L1) / double(misses_L1 + hits_L1);
    miss_rate_L1vc = miss_rate_L2 + (double(misses_L1vc
    )/ double(misses_L1vc + hits_vc));
    misses_L1vc = misses_L1 + misses_L1vc;
    hits_L1vc = hits_L1 + hits_vc;



    cout << line_str << endl;
    cout << "Cache parameters:" << endl;
    cout << line_str << endl;
    cout << "L1 Cache Size (KB):\t\t" << tam_cache_kb_L1 << endl;
    cout << "Cache L1 Associativity:\t\t" << n_way_L1 << endl;
    cout << "Cache Block Size (bytes):\t" << tam_bloque_b << endl;
    cout << line_str << endl;
    cout << "Simulation results:" << endl;
    cout << line_str << endl;
    cout << "Miss rate (L1 + VC):\t\t" << miss_rate_L1vc << endl;
    cout << "Misses (L1 + VC):\t\t" << misses_L1vc << endl;
    cout << "Hits (L1 + VC):\t\t\t" << hits_L1vc << endl;
    cout << "Victim cache hits:\t\t" << hits_vc << endl;
    cout << "Dirty Evictions:\t\t" << dirty_evictions << endl;
    cout << line_str << endl << endl;
  } else {
    // L1
    // Start simulation
    while (ctrl == "#") {
      ctrl = "N";
      cin >> ctrl;
      cin >> op;
      cin >> hex >> address;
      cin >> dec >> ic;

      //Gets tag and index from address
      address_tag_idx_get(address, tam_tag_L1, tam_idx_L1, tam_offset_L1,
                          &index_L1, &tag_L1);

      //Se busca dato en L1
      lru_replacement_policy(  index_L1, tag_L1,  n_way_L1, op,
                                   cache_L1[index_L1], &op_result_L1);

      if(op_result_L1.miss_hit == HIT_STORE || op_result_L1.miss_hit == HIT_LOAD){
        // L1 Hit
        hits_L1++;
      } else {
        // L1 Miss
        misses_L1++;
      }
      if(op_result_L1.dirty_eviction){
          dirty_evictions++;

      }

      //if(cpu_time > 1000){break;}
    }//End while

    //Get cache simulation stats
    miss_rate_L1 = double(misses_L1) / double(misses_L1 + hits_L1);

    cout << line_str << endl;
    cout << "Cache parameters:" << endl;
    cout << line_str << endl;
    cout << "L1 Cache Size (KB):\t\t" << tam_cache_kb_L1 << endl;
    cout << "Cache L1 Associativity:\t\t" << n_way_L1 << endl;
    cout << "Cache Block Size (bytes):\t" << tam_bloque_b << endl;
    cout << line_str << endl;
    cout << "Simulation results:" << endl;
    cout << line_str << endl;
    cout << "Miss rate (L1):\t\t\t" << miss_rate_L1 << endl;
    cout << "Misses (L1):\t\t\t" << misses_L1 << endl;
    cout << "Hits (L1):\t\t\t" << hits_L1 << endl;
    cout << "Dirty Evictions:\t\t" << dirty_evictions << endl;
    cout << line_str << endl << endl;
  }

  return 0;
}
