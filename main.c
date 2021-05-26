#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fileio.h"
#include "keyword.h"
#include "errhandle.h"
#include "identifier.h"
#include "blocks.h"
#define BUFFER_SIZE 255
int main(int argc, char** argv) {
    // Read and Write pointers
    char *fPath;
    char *wPath;
    // END

    // Lexical analyzer variables
    unsigned char flags = LINE_ENDED;
    char expectedKeyCode = -1;
    // END

    // Argument Control
    if (argc == 1) {
        fprintf(stderr, "You must enter file");
        exit(5);
    } else if (argc > 2) {
        fprintf(stderr, "Do not enter more than one argument");
        exit(5);
    } else {
        fPath = *(argv + 1);
        size_t len = strlen(fPath);
        wPath = (char*) malloc((sizeof(char) * len ) + 1);
        strcpy(wPath, fPath);
        len = strlen(wPath);
        wPath[len - 1] = 'x';
        wPath[len - 2] = 'l';
    }
    // END Argument Control

    // All Key Words
    KeyWord* currKeyWord;
    KeyWord *keyWordRoot;
    keyWordRoot = createKeyWordLinkedList();
    // END

    // IdentifierKeeper
    IdentifierKeeper * identifierKeeper;
    identifierKeeper = createIdentifierKeeper();
    // END

    // BlockKeeper
    BlockKeeper* blockKeeper;
    blockKeeper = createBlockKeeper();
    // END

    // Files
    FILE *fPtr = openFile(fPath, "r");
    FILE *wPtr = openFile(wPath, "w");

    // Tracker for current line in Read pointer
    LineTracker *tracker;
    tracker = createLineTracker();
    // END

    char *currWord;
    currWord = malloc(BUFFER_SIZE);

    while(true){
        // End of file check
        if(feof(fPtr)) break;
        // Gets next word from file
        getWord(currWord, fPtr, tracker, BUFFER_SIZE);
        // Try to get KeyWord according to currWord
        currKeyWord = getKeyWord(currWord, keyWordRoot);
        // If current state of lexical analyzer does not expects keyword
        if((flags & KEYWORD_EXPECTED) == 0){
            // Is currWord a keyWord
            if(currKeyWord){
                /*
                 * If currWord is a keyWord
                 * 1-) This is a Error
                 * 2-) KeyWord is "newline"
                 * "newline" is a Key Word but lexical analyzer treats as a not keyword because of
                 * flags. "newline" KeyWord only can come after "out" keyword (flags==OUT_LIST)
                 */
                // keycode 9 is keycode of "newline"
                if(!(currKeyWord->keycode == 9 && flags == OUT_LIST)){
                    err("Unexpected keyword \"%s\", at line, %d\n", currWord, getLine(tracker), &flags,
                        &expectedKeyCode, fPtr);
                    continue;
                }
                fprintf(wPtr, "Keyword newline\n");
                flags = currKeyWord->flagsForNextWord;
                expectedKeyCode = currKeyWord->expectedKeycode;
            }

            // Is currWord identifier and is it expected?
            else if(isIdentifier(currWord) && (flags & IDENTIFIER_EXPECTED) != 0){
                // Are we declaring a identifier?
                if((flags & IDENTIFIER_DECLARE) == IDENTIFIER_DECLARE){
                    // Declare identifier if not. If it's already declared gives error
                    if(!declareIdentifier(currWord, identifierKeeper)){
                        err("Identifier \"%s\" already declared. Error at line %d\n", currWord, getLine(tracker),
                            &flags, &expectedKeyCode, fPtr);
                        continue;
                    }
                }
                // Are we using a identifier?
                else if((flags & IDENTIFIER_USE) == IDENTIFIER_USE){
                    // Has identifier declared?
                    if(!isIdentifierDeclared(currWord, identifierKeeper)){
                        err("Identifier \"%s\" has not declared. Error at line %d\n", currWord, getLine(tracker),
                            &flags, &expectedKeyCode, fPtr);
                        continue;
                    }
                }
                fprintf(wPtr, "Identifier %s\n", currWord);
            }
            else if(isIntConstant(currWord) && (flags & INT_EXPECTED) == INT_EXPECTED)
                fprintf(wPtr, "IntConstant %s\n", currWord);
            else if(isStringConstant(currWord, tracker) && (flags & STRING_EXPECTED) == STRING_EXPECTED)
                fprintf(wPtr, "StringConstant %s\n", currWord);
            else{
                err("Unrecognized character \"%s\", at line %d\n", currWord, getLine(tracker), &flags,
                    &expectedKeyCode, fPtr);
                continue;
            }
            flags = flags == OUT_LIST?SEPERATOR_EXPECTED:NOP;
        }
        // If keyword expected (LINE_ENDED or NOP)
        else if(currKeyWord && (flags & KEYWORD_EXPECTED) != 0){
            switch (currKeyWord->keycode) {
                case 0:
                    fprintf(wPtr, "Keyword int\n");
                    break;
                case 1:
                    fprintf(wPtr, "Keyword move\n");
                    break;
                case 2:
                    fprintf(wPtr, "Keyword add\n");
                    break;
                case 3:
                    fprintf(wPtr, "Keyword sub\n");
                    break;
                case 4:
                    fprintf(wPtr, "Keyword out\n");
                    break;
                case 5:
                    fprintf(wPtr, "Keyword loop\n");
                    break;
                case 6:
                    if((flags & BLOCK_EXPECTED) == BLOCK_EXPECTED){
                        openBlock(blockKeeper, getLine(tracker));
                        fprintf(wPtr, "OpenBlock\n");
                    }
                    else
                        err("Unexpected keyword \"%s\", at line, %d\n", currWord, getLine(tracker), &flags,
                            &expectedKeyCode, fPtr);
                    break;
                case 7:
                    if(closeBlock(blockKeeper))
                        fprintf(wPtr, "CloseBlock\n");
                    else
                        err("Unexpected keyword \"%s\", at line, %d\n", currWord, getLine(tracker), &flags,
                            &expectedKeyCode, fPtr);
                    break;
                case 8:
                    fprintf(wPtr, "Keyword times\n");
                    break;
                case 9:
                    fprintf(wPtr, "Keyword newline\n");
                    break;
                case 10:
                    fprintf(wPtr, "Keyword to\n");
                    break;
                case 11:
                    fprintf(wPtr, "EndOfLine\n");
                    break;
                case 12:
                    fprintf(wPtr, "Separator\n");
                    break;
                case 13:
                    fprintf(wPtr, "Keyword from\n");
                    break;
                default:
                    fprintf(stderr, "Unexpected error! Exiting\n");
                    exit(-1);
            }
            flags = currKeyWord->flagsForNextWord;
            expectedKeyCode = currKeyWord->expectedKeycode;
        }
    }
    while (blockKeeper->totalBlocks != 0){
        fprintf(stderr, "Block(line, %d) left open!\n", closeBlockAndGetLine(blockKeeper));
    }
    if(flags != LINE_ENDED){
        fprintf(stderr, "No \"End line\"(\".\") character found at line %d\n", getLine(tracker));
    }

    freeIdentifierKeeper(identifierKeeper);
    freeBlockKeeper(blockKeeper);
    freeKeyWordLinkedList(keyWordRoot);
    free(tracker);
    free(currWord);
    free(wPath);
    fclose(fPtr);
    fclose(wPtr);
    return 0;
}
