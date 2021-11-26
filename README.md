## Cache Memory Simulation
### Joshua Beaty
### Description:

The program reads a data file of instructions and addresses. This program ignores the insructions and focuses on the addresses. It checks the cache memory and victim cache to see if the address is there. If it is, it updates the access time. If it isn't in either the main cache or victim's cache. it places the address in the main cache and records the miss.

After the entire data file has been read, the program prints the total number of misses to the console.

#### Example

- Input: 
    - 2 40bc74
    - 0 7ffebac8
    - 2 40bc78
- Output: 
    - Total number of Cache misses: 2

### Files

|   #   | File                       | Description                                                |
| :---: | -------------------------- | ---------------------------------------------------------- |
|   1   | [main.cpp](./main.cpp)     | My solution file                                           |
|   2   | [data-file-spice.txt](./data-file-spice.txt)     | Input file supplied by Dr. Passos    |
|   3   | [debug.out](./debug.out)   | Output file that somewhat tracks the program's progress    |
|   4   | [output.PNG](./output.PNG) | Screenshot showing the program's final output.             |
### Instructions

- Update filename on line 98 of main.cpp if you wish to run it on another input file.
- recompile the main.cpp
- make sure the input file specified is in the same folder as the executable.
- program takes a approximately 30-45 seconds to run to completion.