//
// Created by ensar on 5/26/2021.
//

#ifndef LEXICAL_WIN_BLOCKS_H
#define LEXICAL_WIN_BLOCKS_H

#include <stdlib.h>
#include <stdbool.h>
#include "bigint.h"

// BlockKeeper
/*
 * Keeps all opened blocks and where it opened
 * It uses Linked List as Structure
 * When two blocks are nested, nested block linked inside first block
 * Block:
 *  - lineStarted: Blocks' start line.
 *  - nests: Points nested block.
 * BlockKeeper:
 *  - root: Root of linked list.
 *  - totalBlocks: Count of currently open blocks
 */

typedef struct blockNode {
    unsigned int lineStarted;
    long long int fPointer;
    BigInt *loopCounter;
    bool isIntConstant;
    bool isShortHandLoop;
    struct blockNode *nests;
} Block;

typedef struct blockKeeper {
    unsigned int totalBlocks;
    Block *root;
} BlockKeeper;


void openBlock(BlockKeeper *blockKeeper, BigInt *loopCounter, unsigned int line, unsigned long long int fPointer, bool isIntConstant, bool isShortHandLoop);

Block* getBlock(BlockKeeper* blockKeeper);

bool closeBlock(BlockKeeper *blockKeeper);

int closeBlockAndGetLine(BlockKeeper *blockKeeper);

BlockKeeper *createBlockKeeper();

void freeBlockKeeper(BlockKeeper *blockKeeper);

#endif //LEXICAL_WIN_BLOCKS_H
