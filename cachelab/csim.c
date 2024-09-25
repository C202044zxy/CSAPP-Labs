#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "cachelab.h"
#define ull unsigned long

int s, S, E, b;
int miss, hit, evict; 
char fileName[105];
typedef struct {
    int valid; 
    ull tag;
    int lastTime; 
} Cache; 

Cache **cache; 

void initCache() {
    cache = (Cache **) malloc(S * sizeof(Cache *));
    for(int i = 0; i < S; i++) {
        cache[i] = (Cache *) malloc(E * sizeof(Cache)); 
        memset(cache[i], 0, E * sizeof(Cache)); 
    }
}

void freeCache() {
    for(int i = 0; i < E; i++) {
        free(cache[i]);
    }
    free(cache); 
}

void fetchAddr(ull addr, int timeStamp) {
    ull tag = addr >> (s + b); 
    int ind = (addr >> b) & ((1 << s) - 1);
    // int offset = addr & ((1 << b) - 1);
    Cache* cacheSet = cache[ind]; 

    for(int i = 0; i < E; i++) {
        if(cacheSet[i].valid && cacheSet[i].tag == tag) {
            hit++;
            cacheSet[i].lastTime = timeStamp; 
            return ;
        }
    }

    miss++;
    for(int i = 0; i < E; i++) {
        if(!cacheSet[i].valid) {
            cacheSet[i].valid = 1;
            cacheSet[i].tag = tag;
            cacheSet[i].lastTime = timeStamp;
            return ; 
        }
    }

    int id = 0, least = 0x3f3f3f3f;
    for(int i = 0; i < E; i++) {
        if(cacheSet[i].lastTime < least) {
            least = cacheSet[i].lastTime; 
            id = i;
        }
    }
    evict ++; 
    // replace id
    cacheSet[id].lastTime = timeStamp; 
    cacheSet[id].tag = tag;
    cacheSet[id].valid = 1;
    return ;
}

void readFile() {
    FILE* file = fopen(fileName, "r"); 
    char opt[5];
    ull address = 0;
    int size = 0, timeStamp = 0;
    while((fscanf(file, "%s %lx,%d", opt, &address, &size)) != -1) {
        timeStamp ++; 
        switch (opt[0]) {
            case 'M':
                fetchAddr(address, timeStamp);
                fetchAddr(address, timeStamp);
                break; 
            case 'L':
            case 'S':
                fetchAddr(address, timeStamp);
                break;
        }
    }
    printSummary(hit, miss, evict); 
    fclose(file); 
}

int main(int argc, char *argv[]) {
    char opt = 0;
    while((opt = getopt(argc, argv, "s:E:b:t:")) != -1) {
        switch(opt) {
            case 's':
                s = atoi(optarg); // global variable in <unistd.h>
                S = 1 << s;
                break; 
            case 'E':
                E = atoi(optarg); 
                break;
            case 'b':
                b = atoi(optarg); 
                break; 
            case 't': 
                strcpy(fileName, optarg); 
                break; 
        }
    }
    initCache(); 
    readFile(); 
    freeCache(); 
}