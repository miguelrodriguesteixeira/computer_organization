#include "L2_Associative_Cache.h"
#include "Cache.h"


uint8_t L1CacheData[L1_SIZE];
uint8_t L2CacheData[L2_SIZE];
uint8_t DRAM[DRAM_SIZE];
uint32_t time;
Cache_L1 L1Cache;
Cache_L2 L2Cache;


/**************** Time Manipulation ***************/
void resetTime() { time = 0; }

uint32_t getTime() { return time; }

/****************  RAM memory (byte addressable) ***************/
void accessDRAM(uint32_t address, uint8_t *data, uint32_t mode) {

    if (address >= DRAM_SIZE - WORD_SIZE + 1)
        exit(-1);

    if (mode == MODE_READ) {
        memcpy(data, &(DRAM[address]), BLOCK_SIZE);
        time += DRAM_READ_TIME;
    }

    if (mode == MODE_WRITE) {
        memcpy(&(DRAM[address]), data, BLOCK_SIZE);
        time += DRAM_WRITE_TIME;
    }
}

/*********************** L1 cache *************************/

void initCache() {
    for (int i = 0; i < (L1_LINES); i++)
        L1Cache.line[i].Valid = 0;

    for (int i = 0; i < L2_LINES; i++) {
        for (int j = 0; j < L2_ASSOCIATIVITY; j++) {
            L2Cache.lines[i].sets[j].Valid = 0;
        }
    }
}


void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {

    uint32_t index, Tag, offset;

    Tag = address / L1_SIZE;
    index = (address / BLOCK_SIZE) % (L1_LINES);
    offset = address % BLOCK_SIZE;

    if (L1Cache.line[index].Valid && L1Cache.line[index].Tag == Tag) {
        // Hit
        if (mode == MODE_READ) {
            memcpy(data, &(L1CacheData[index * BLOCK_SIZE + offset]), WORD_SIZE);

            time += L1_READ_TIME;
        }

        if (mode == MODE_WRITE) {
            memcpy(&(L1CacheData[index * BLOCK_SIZE + offset]), data, WORD_SIZE);

            time += L1_WRITE_TIME;
            L1Cache.line[index].Dirty = 1;
        }
    } else {
        // Miss

        if (L1Cache.line[index].Valid && L1Cache.line[index].Dirty) {
            accessL2(L1Cache.line[index].Tag * L1_LINES + index * BLOCK_SIZE, &(L1CacheData[index * BLOCK_SIZE]),
                     MODE_WRITE);
        }


        accessL2(address - offset, &(L1CacheData[index * BLOCK_SIZE]), MODE_READ);
        L1Cache.line[index].Valid = 1;
        L1Cache.line[index].Tag = Tag;
        L1Cache.line[index].Dirty = 0;

        if (mode == MODE_READ) {
            memcpy(data, &(L1CacheData[index * BLOCK_SIZE + offset]), WORD_SIZE);
            time += L1_READ_TIME;
        } else if (mode == MODE_WRITE) {
            memcpy(&(L1CacheData[index * BLOCK_SIZE + offset]), data, WORD_SIZE);
            time += L1_WRITE_TIME;
            L1Cache.line[index].Dirty = 1;
        }
    }
}

void accessL2(uint32_t address, uint8_t *data, uint32_t mode) {
    uint32_t index, Tag, offset;

    Tag = address / (L2_SIZE / (BLOCK_SIZE * L2_ASSOCIATIVITY) * BLOCK_SIZE);
    index = (address / BLOCK_SIZE) % (L2_SIZE / (BLOCK_SIZE * L2_ASSOCIATIVITY));
    offset = address % BLOCK_SIZE;

    int leastRecentlyUsedIndex = -1;
    uint32_t leastRecentlyUsedTime = UINT32_MAX;

    for (int i = 0; i < L2_ASSOCIATIVITY; i++) {
        if (L2Cache.lines[index].sets[i].Valid && L2Cache.lines[index].sets[i].Tag == Tag) {
            // Cache Hit
            if (mode == MODE_READ) {
                memcpy(data, &(L2CacheData[index * BLOCK_SIZE * L2_ASSOCIATIVITY + i * BLOCK_SIZE + offset]), WORD_SIZE);
                time += L2_READ_TIME;
            } else if (mode == MODE_WRITE) {
                memcpy(&(L2CacheData[index * BLOCK_SIZE * L2_ASSOCIATIVITY + i * BLOCK_SIZE + offset]), data, WORD_SIZE);
                time += L2_WRITE_TIME;
                L2Cache.lines[index].sets[i].Dirty = 1;
            }

            // Update LRU information
            L2Cache.lines[index].sets[i].Time = time;

            return;
        }

        // Find the least recently used line
        if (L2Cache.lines[index].sets[i].Time < leastRecentlyUsedTime) {
            leastRecentlyUsedTime = L2Cache.lines[index].sets[i].Time;
            leastRecentlyUsedIndex = i;
        }
    }

    // Cache Miss
    for (int i = 0; i < L2_ASSOCIATIVITY; i++) {
        if (L2Cache.lines[index].sets[i].Valid && L2Cache.lines[index].sets[i].Dirty) {
            accessDRAM(L2Cache.lines[index].sets[i].Tag * L2_LINES + index * BLOCK_SIZE * L2_ASSOCIATIVITY,
                       &(L2CacheData[index * BLOCK_SIZE * L2_ASSOCIATIVITY + i * BLOCK_SIZE]), MODE_WRITE);
            memset(&(L2CacheData[index * BLOCK_SIZE * L2_ASSOCIATIVITY + i * BLOCK_SIZE]), 0, BLOCK_SIZE);
        }
    }

    accessDRAM(address - offset, &(L2CacheData[index * BLOCK_SIZE * L2_ASSOCIATIVITY]), MODE_READ);

    L2Cache.lines[index].sets[leastRecentlyUsedIndex].Valid = 1;
    L2Cache.lines[index].sets[leastRecentlyUsedIndex].Tag = Tag;
    L2Cache.lines[index].sets[leastRecentlyUsedIndex].Dirty = 0;
    L2Cache.lines[index].sets[leastRecentlyUsedIndex].Time = time;

    if (mode == MODE_READ) {
        memcpy(data, &(L2CacheData[index * BLOCK_SIZE * L2_ASSOCIATIVITY + leastRecentlyUsedIndex * BLOCK_SIZE + offset]),
               WORD_SIZE);
        time += L2_READ_TIME;
    } else if (mode == MODE_WRITE) {
        memcpy(&(L2CacheData[index * BLOCK_SIZE * L2_ASSOCIATIVITY + leastRecentlyUsedIndex * BLOCK_SIZE + offset]), data,
               WORD_SIZE);
        time += L2_WRITE_TIME;
        L2Cache.lines[index].sets[leastRecentlyUsedIndex].Dirty = 1;
    }
}


void read(uint32_t address, uint8_t *data) {
    accessL1(address, data, MODE_READ);
}

void write(uint32_t address, uint8_t *data) {
    accessL1(address, data, MODE_WRITE);
}
