/*
Cache Memory Program
Joshua Beaty
11/25/2021

This program reads a file of hexadecimal addresses preceded by a 
single digit instruction. in the format:
0 40ac501
2 45adc07

The program decodes the hexadecimal address and determines 
if the address is in the cache memory or not and records the number of cache misses.

4kb Cache Size
64 byte Block Size
32 Sets
2 blocks per set

32 bit address breakdown
Tag        Set       Offset
21 bits    5 bits    6 bits
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>

using namespace std;

//global variables
//When debugMode is true, prints extra output to debug.out file
bool debugMode = true;
ofstream debug;

//Cache Block struct keeps track of the block's tag
//and when it was last accessed.
struct cacheBlock
{
    int lastAccessStep;
    string tag;

    cacheBlock()
    {
        lastAccessStep = 0;
        tag = "";
    }
    cacheBlock(int s, string t)
    {
        lastAccessStep = s;
        tag = t;
    }
};

struct cacheSet
{
    vector<cacheBlock *> blocks;

    cacheSet()
    {
        cacheBlock *tempA = new cacheBlock();
        cacheBlock *tempB = new cacheBlock();
        blocks.push_back(tempA);
        blocks.push_back(tempB);
    }
};

struct cacheAddress
{
    string tag;
    int set;
    int offset;
    cacheAddress()
    {
        tag = "";
        set = 0;
        offset = 0;
    }
    cacheAddress(string t, int s, int o)
    {
        tag = t;
        set = s;
        offset = o;
    }
};

//function prototypes
cacheAddress *parseAddress(string);
string HexToBin(char);

int main()
{
    //initialize variables
    ifstream fin;
    int step = 0;
    int misses = 0;
    string filename = "data-file-spice.txt";
    string instruction = "";
    vector<cacheBlock *> victimCache;
    vector<cacheSet *> Cache;

    //open the files
    fin.open(filename);
    debug.open("debug.out");

    if (debugMode)
        debug << "==Cache Memory Program Initialized==\n\tOpened: " << filename << "\n";

    //fill the vector
    for (int a = 0; a < 32; a++)
    {
        cacheSet *tempSet = new cacheSet();
        Cache.push_back(tempSet);
    }

    for (int b = 0; b < 2; b++)
    {
        cacheBlock *tempBlock = new cacheBlock();
        victimCache.push_back(tempBlock);
    }

    if (debugMode)
    {
        debug << "\tCache size: " << Cache.size() << " sets\n";
        debug << "\tNumber of Blocks: " << Cache.size() * Cache[0]->blocks.size() << "\n";
        debug << "\tVictim Size: " << victimCache.size() << " blocks\n\n";
        debug << "==Beginning File Read==\n";
    }

    //read the file
    while (fin >> instruction)
    {
        step++;
        string input;
        cacheAddress *address;

        //read the address
        fin >> input;
        if (debugMode)
        {
            debug << "=Read Line #" << step << "=\n";
            debug << "\tAddress: " << input << "\n";
        }

        //make the address size 8 if less than 8
        if (input.length() < 8)
            for (int c = 0; c <= 8 - input.length(); c++)
                input.insert(0, "0");

        if (debugMode)
        {
            debug << "\tResized Address: " << input << "\n";
            debug << "=Parsing Address=\n";
        }

        //decode the address
        address = parseAddress(input);
        if (debugMode)
        {
            debug << "\tTag: " << address->tag << "\n";
            debug << "\tSet: " << address->set << "\n";
            debug << "\tOffset: " << address->offset << "\n";
        }

        bool tagFound = false;
        int blockLoc = -1;
        //check the set for the tag
        for (int b2 = 0; b2 < 2; b2++)
        {
            if (Cache[address->set]->blocks[b2]->tag == address->tag)
            {
                tagFound = true;
                blockLoc = b2;
            }
        }

        int victimLocation = -1;
        //if tag isn't found
        //check victim cache
        if (!tagFound)
        {
            for (int c = 0; c < 2; c++)
            {
                if (Cache[address->set]->blocks[c]->tag == address->tag)
                {
                    tagFound = true;
                    victimLocation = c;
                }
            }
            //if tag still isn't found
            //increment misses
            //if block isn't empty, move current block to victim cache
            //add address block to cache
            //update current step on main cache block
            if (!tagFound)
            {
                misses++;
                int empty = -1;
                for (int d = 0; d < 2; d++)
                    if (Cache[address->set]->blocks[d]->tag == "")
                        empty = d;
                //If no empty block
                if (empty < 0)
                {
                    //delete oldest element in victim cache
                    victimCache.erase(victimCache.begin());
                    //find LRU and put it on victim cache.
                    if (Cache[address->set]->blocks[0]->lastAccessStep < Cache[address->set]->blocks[1]->lastAccessStep)
                    {
                        cacheBlock *newVictim = new cacheBlock(Cache[address->set]->blocks[0]->lastAccessStep, Cache[address->set]->blocks[0]->tag);
                        victimCache.push_back(newVictim);
                        Cache[address->set]->blocks[0]->tag = address->tag;
                        Cache[address->set]->blocks[0]->lastAccessStep = step;
                    }
                    else
                    {
                        cacheBlock *newVictim = new cacheBlock(Cache[address->set]->blocks[1]->lastAccessStep, Cache[address->set]->blocks[1]->tag);
                        victimCache.push_back(newVictim);
                        Cache[address->set]->blocks[1]->tag = address->tag;
                        Cache[address->set]->blocks[1]->lastAccessStep = step;
                    }
                }
                else
                {
                    //Found empty block
                    //add address tag to cache block
                    Cache[address->set]->blocks[empty]->tag = address->tag;
                    //update last access step to cache block
                    Cache[address->set]->blocks[empty]->lastAccessStep = step;
                }
            }
            //if tag is found in victim cache, move the victim cache block
            //into main cache and put the main cache block in the victim cache
            //update current step on main cache block
            else
            {
                string victimTag = "";
                int newVictim;
                victimTag = victimCache[victimLocation]->tag;

                //find LRU
                if (Cache[address->set]->blocks[0]->lastAccessStep < Cache[address->set]->blocks[1]->lastAccessStep)
                {
                    newVictim = 0;
                }
                else
                {
                    newVictim = 1;
                }

                //put main cache block where victim was.
                victimCache[victimLocation]->tag = Cache[address->set]->blocks[newVictim]->tag;
                victimCache[victimLocation]->lastAccessStep = Cache[address->set]->blocks[newVictim]->lastAccessStep;

                //put victim in main cache block
                Cache[address->set]->blocks[newVictim]->tag = victimTag;
                Cache[address->set]->blocks[newVictim]->lastAccessStep = step;
            }
        }
        else
        { //tag is found, update last accessed step
            Cache[address->set]->blocks[blockLoc]->lastAccessStep = step;
        }

        if (debugMode)
            debug << "\n";
    }

    //print out results
    cout << "Total number of Cache misses: " << misses << "\n";

    //clear the vectors
    victimCache.clear();
    Cache.clear();

    //close the files
    fin.close();
    debug.close();

    //terminate program
    return 0;
}

/*
32 bit address breakdown
Tag        Set       Offset
21 bits    5 bits    6 bits
*/
cacheAddress *parseAddress(string a)
{
    cacheAddress *rtn = new cacheAddress();
    string temp = "";
    for (char c : a)
    {
        temp += HexToBin(c);
    }
    if (debugMode)
        debug << "\tParsed Address: " << temp << "\n";
    for (int i = 0; i < 21; i++)
        rtn->tag += temp[i];

    string setText = "";
    for (int j = 21; j < 26; j++)
    {
        setText += temp[j];
    }
    if (debugMode)
        debug << "\tSet bits: " << setText << "\n";
    for (int k = 0; k < setText.length(); k++)
    {
        if (setText[k] > '0')
            rtn->set += pow(2, setText.length() - (k + 1));
    }

    string offsetText = "";
    for (int y = 26; y < temp.length(); y++)
    {
        offsetText += temp[y];
    }
    if (debugMode)
        debug << "\tOffset bits: " << offsetText << "\n";
    for (int z = 0; z < offsetText.length(); z++)
    {
        if (offsetText[z] > '0')
            rtn->offset += pow(2, offsetText.length() - (z + 1));
    }

    return rtn;
}

string HexToBin(char c)
{
    string temp = "";
    switch (c)
    {
    case '0':
        temp += "0000";
        break;
    case '1':
        temp += "0001";
        break;
    case '2':
        temp += "0010";
        break;
    case '3':
        temp += "0011";
        break;
    case '4':
        temp += "0100";
        break;
    case '5':
        temp += "0101";
        break;
    case '6':
        temp += "0110";
        break;
    case '7':
        temp += "0111";
        break;
    case '8':
        temp += "1000";
        break;
    case '9':
        temp += "1001";
        break;
    case 'a':
        temp += "1010";
        break;
    case 'b':
        temp += "1011";
        break;
    case 'c':
        temp += "1100";
        break;
    case 'd':
        temp += "1101";
        break;
    case 'e':
        temp += "1110";
        break;
    case 'f':
        temp += "1111";
        break;
    }
    return temp;
}