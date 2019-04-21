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
  cerr << "Uso: " << "cache" << " -t <Tamaño Cache (kB)> -l <Tamaño Bloque (B)> -a <Asociatividad> -rp <PR>" << endl;
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

int main(int argc, char * argv []) {

  /* Cache variables */
  /* From command line */
  int tam_cache_kb = 0;
  int tam_bloque_b = 0;
  int asociatividad = 0;
  replacement_policy rp = RANDOM;
  /* From functions */
  int tam_tag = 0;
  int tam_idx = 0;
  int tam_offset = 0;
  /* From simulation */
  long long cpu_time = 0;
  int amat = 0;
  double miss_rate_total = 0;
  double miss_rate_read = 0;
  int dirty_evictions = 0;
  int misses_load = 0;
  int misses_store = 0;
  int misses_total = 0;
  int hits_load = 0;
  int hits_store = 0;
  int hits_total = 0;
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
        asociatividad = stoi(argv[i+1]);
      } else {
        print_usage();
      }
    }
    if (arg == "-rp") {
      if(i + 1 < argc){
        string argRP = argv[i+1];
        //Assigns replacement policy
        if (argRP == "LRU" || argRP == "lru") {
          rp = LRU;
        } else if (argRP == "RRIP" || argRP == "rrip") {
          rp = RRIP;
        } else {  //Default: RANDOM
          rp = RANDOM;
        }
      } else {
        print_usage();
      }
    }
  }
  // Checks if all parameters are assigned
  if(tam_cache_kb == 0 || tam_bloque_b == 0 || asociatividad == 0 || rp == RANDOM){
    print_usage();
  }
  // Getting tag, index and offset sizes
  field_size_get( tam_cache_kb, asociatividad, tam_bloque_b,
                  &tam_tag, &tam_idx, &tam_offset);

  /* Get trace's lines and start your simulation */
  string ctrl = "#";
  bool op;
  long long address;
  int ic;
  long accesos = 0;
  long load_acc = 0;
  long store_acc = 0;

  cout << endl;
  while (ctrl == "#") {
    ctrl = "N";
    cin >> ctrl;
    cin >> op;
    cin >> hex >> address;
    cin >> dec >> ic;


    //if(load_acc > 100){break;}
    cpu_time = cpu_time + ic;

    if(op){
      store_acc++;
    } else{
      load_acc++;
    }

  }//End while

  accesos = store_acc + load_acc;
  cout << "Accesos:\t" << accesos << endl;
  cout << "Load:\t" << load_acc << endl;
  cout << "Store:\t" << store_acc << endl;






  /* Print cache configuration */

  cout << endl;
  cout << "RP:\t\t" << rp << endl;
  cout << "Index size:\t" << tam_idx << endl;
  cout << "Offset size:\t" << tam_offset << endl;
  cout << "Tag size:\t" << tam_tag << endl;
  cout << endl;

  /* Print Statistics */
  string line_str =  "--------------------------------------------";
  string line_str0 = "____________________________________________";
  cout << line_str << endl;
  cout << "Cache parameters:" << endl;
  cout << line_str << endl;
  cout << "Cache Size (KB):\t\t" << tam_cache_kb << endl;
  cout << "Cache Associativity:\t\t" << asociatividad << endl;
  cout << "Cache Block Size (bytes):\t" << tam_bloque_b << endl;
  cout << line_str << endl;
  cout << "Simulation results:" << endl;
  cout << line_str << endl;
  cout << "CPU time (cycles):\t\t" << cpu_time << endl;
  cout << "AMAT (cycles):\t\t\t" << amat << endl;
  cout << "Overall miss rate:\t\t" << miss_rate_total << endl;
  cout << "Read miss rate:\t\t\t" << miss_rate_read << endl;
  cout << "Dirty Evictions:\t\t" << dirty_evictions << endl;
  cout << "Load misses:\t\t\t" << misses_load << endl;
  cout << "Store misses:\t\t\t" << misses_store << endl;
  cout << "Total misses:\t\t\t" << misses_total << endl;
  cout << "Load hits:\t\t\t" << hits_load << endl;
  cout << "Store hits:\t\t\t" << hits_store << endl;
  cout << "Total hits:\t\t\t" << hits_total << endl;
  cout << line_str << endl << endl;

return 0;
}
