# Cache simulator IE0521 project

Code based on the base line that can be found in [here](https://github.com/lsanabri/cache_simulator_IE521_baseline).

## How to build the project
Build the project from the *src* folder
```
>> make
```
## How to run
Again, in *src* folder
```
>> make run
```
Or explicit
```
>> gunzip -c mcf.trace.gz | ./cache -a <associativity> -t <cache size KB> -l <block size in bytes> -rp <replacement policy>
```

For example
```
>> gunzip -c mcf.trace.gz | ./cache -a 4 -t 16 -l 32 -rp LRU
```

## To clean
In *src* folder
```
>> make clean
```
