#include "L1Cache.h"
#include "Cache.h"


uint8_t L1Cache[L1_SIZE]; // L1 cache with multiple lines
uint8_t L2Cache[L2_SIZE];
uint8_t DRAM[DRAM_SIZE];
uint32_t time;
Cache SimpleCache;

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
        SimpleCache.line[i].Valid = 0;
}

void accessL1(uint32_t address, uint8_t *data, uint32_t mode) {
    uint32_t index, Tag, offset;

    Tag = address / L1_SIZE;
    index = (address / BLOCK_SIZE) % (L1_LINES);
    offset = address % BLOCK_SIZE;

    if (SimpleCache.line[index].Valid && SimpleCache.line[index].Tag == Tag) {
        // Hit on L1Cache
        if (mode == MODE_READ) {
            memcpy(data, &(L1Cache[index * BLOCK_SIZE + offset]), WORD_SIZE);
            time += L1_READ_TIME;
        } else if (mode == MODE_WRITE) {
            memcpy(&(L1Cache[index * BLOCK_SIZE + offset]), data, WORD_SIZE);
            time += L1_WRITE_TIME;
            SimpleCache.line[index].Dirty = 1;
        }
    } else {
        // Miss on L1Cache
        if (SimpleCache.line[index].Valid && SimpleCache.line[index].Dirty) {
            accessDRAM(SimpleCache.line[index].Tag * L1_LINES + index * BLOCK_SIZE, &(L1Cache[index * BLOCK_SIZE]),
                       MODE_WRITE);
        }
        accessDRAM(address - offset, &(L1Cache[index * BLOCK_SIZE]), MODE_READ);

        SimpleCache.line[index].Valid = 1;
        SimpleCache.line[index].Tag = Tag;
        SimpleCache.line[index].Dirty = 0;

        // Update L1Cache with the data
        if (mode == MODE_READ) {
            memcpy(data, &(L1Cache[index * BLOCK_SIZE + offset]), WORD_SIZE);
            time += L1_READ_TIME;
        } else if (mode == MODE_WRITE) {
            memcpy(&(L1Cache[index * BLOCK_SIZE + offset]), data, WORD_SIZE);
            time += L1_WRITE_TIME;
            SimpleCache.line[index].Dirty = 1;
        }
    }
}


    void read(uint32_t address, uint8_t *data) {
        accessL1(address, data, MODE_READ);
    }

    void write(uint32_t address, uint8_t *data) {
        accessL1(address, data, MODE_WRITE);
    }
