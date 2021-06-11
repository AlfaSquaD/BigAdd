# Lexical Analyzer for BigAdd Language

## Big Add language

BigAdd language is a small programming language that has been designed in order to add and subtract
big integers programmatically and display the result on the screen.

BigAdd is an interpreted language. The BigAdd interpreter works from the command line with the command BA, and takes the script
file’s name as the only attribute.

example: The command `c:\> ba myscript`
must load and execute the script file called myscript.ba

## Data Structures

### Word

#### Word Struct
```c
typedef struct wordStruct{
    char* word;
    unsigned int size;
}Word;
```
Keeps a string and its size.

#### Word Functions

* ```c 
  Word* createWord();
  ```
  * Creates a Word Struct and returns its pointer.

* ```c 
  void expandWord(Word* word)
  ```
  * Expands char array in given Word. (Size is 2 times of old size)

* ```c 
  void shrinkWord(Word* word)
  ```
  * Shrinks char array in given Word. (Size predefined)

* ```c 
  void freeWord(Word* word);
  ```
  * Frees given Word.


### IdentifierKeeper

#### Identifier Struct
```c
typedef struct identifierNode {
  char *name; // Identifier name
  int val; // Identifiers' value (not used yet)
  struct identifierNode *next; // next Identifier
} Identifier;
```

#### IdentifierKeeper Struct
```c
typedef struct identifierKeeper {
  unsigned int size; // root of Linked List
  Identifier *root; // size of list
} IdentifierKeeper;
```

Keeps all declared Identifiers and their values in Linked List.

#### IdentifierKeeper Functions

* ```c 
  bool isIdentifierDeclared(char *identifierName, IdentifierKeeper *keeper)
  ```
  * Checks given identifier has declared or not.

* ```c 
  bool declareIdentifier(char *identifierName, IdentifierKeeper *keeper);
  ```
  * Declares given identifier name. If it's already declared, returns false.

* ```c 
  IdentifierKeeper *createIdentifierKeeper();
  ```
  * Creates an IdentifierKeeper and returns its pointer.

* ```c 
  void freeIdentifierKeeper(IdentifierKeeper *identifierKeeper);
  ```
  * Frees given IdentifierKeeper.

  

### Variable

#### Variable Struct
```c
typedef struct variableStack{
    DataType dataType;
    void* data;
    struct variableStack *next;
}Variable;
```

Keeps given variables for operations.

#### enum DataType

Data stored in variable as void pointer.
Operation functions casts void pointer according to the dataType.
DataTypes are:
* dIdentifier
* dIntConstant
* dStringConstant

#### Variable Functions

* ```c 
  Variable* addVariable(Variable* curr, void* data, DataType type);
  ```
  * Adds given data to given stack

* ```c 
  void freeVariableStack(Variable* curr);
  ```
  * Frees given variable stack





### IdentifierKeeper

#### Identifier Struct
```c
typedef struct identifierNode {
  char *name; // Identifier name
  int val; // Identifiers' value (not used yet)
  struct identifierNode *next; // next Identifier
} Identifier;
```

#### IdentifierKeeper Struct
```c
typedef struct identifierKeeper {
  unsigned int size; // root of Linked List
  Identifier *root; // size of list
} IdentifierKeeper;
```

Keeps all declared Identifiers and their values in Linked List.

#### IdentifierKeeper Functions

* ```c 
  bool isIdentifierDeclared(char *identifierName, IdentifierKeeper *keeper)
  ```
  * Checks given identifier has declared or not.

* ```c 
  bool declareIdentifier(char *identifierName, IdentifierKeeper *keeper);
  ```
  * Declares given identifier name. If it's already declared, returns false.

* ```c 
  IdentifierKeeper *createIdentifierKeeper();
  ```
  * Creates an IdentifierKeeper and returns its pointer.

* ```c 
  void freeIdentifierKeeper(IdentifierKeeper *identifierKeeper);
  ```
  * Frees given IdentifierKeeper.
  

### Operation functions
Operation functions uses Variable stack and IdentifierKeeper to make operations.

Parser makes sure variables are correct. (No unnecessary data, correct order)

### BlockKeeper
#### Block Struct

```c
typedef struct blockNode {
unsigned int lineStarted;
long int fPointer;
long int *loopCounter;
bool isIntConstant;
bool isShortHandLoop;
struct blockNode *nests;
} Block;
```

#### BlockKeeperStruct

```c
typedef struct blockKeeper {
unsigned int totalBlocks;
Block *root;
} BlockKeeper;
```

Keeps all opened blocks, where they opened(file position ,via fteel function, and line), its loop counter(pointer to the Identifier value or IntConstant).
When two blocks nested, the nested block linked inside the parent block.

* isIntConstant: Is loop counter a IntConstant? If it is, free loop counter after loop.
* isShortHandLoop: Is it a one line loop? If it is, Decrement loopCounter after End of Line.

#### BlockKeeper Functions

* ```c 
  void openBlock(BlockKeeper *blockKeeper, long int *loopCounter, unsigned int line, unsigned long int fPointer,
   bool isIntConstant, bool isShortHandLoop);
  ```
  * Opens a new block. Nests new block if already a block opened.

* ```c 
  bool closeBlock(BlockKeeper *blockKeeper);
  ```
  * Closes the deepest block. If there is no block to close, returns false.

* ```c 
  unsigned int closeBlockAndGetLine(BlockKeeper *blockKeeper);
  ```
  * Closes the deepest block and returns its startLine. If there is no block to close, returns -1.

* ```c 
  BlockKeeper *createBlockKeeper()
  ```
  * Creates a BlockKeeper and returns its pointer.

* ```c 
  void freeBlockKeeper(BlockKeeper *blockKeeper)
  ```
  * Frees given BlockKeeper.

### KeyWord

#### KeyWord Struct

```c
typedef struct keyWordStruct {
    char *keyWord; // keyword
    Keycode expectedKeycode; // Expected keyword after this keyword
    Keycode keycode; // Keywords' keycode
    unsigned char flagsForKeyword; // Expected flag for the use of this keyword
    unsigned char flagsForNextWord; // Flag for next expected word
    void (*operationFunc)(Variable* stack, IdentifierKeeper* identifierKeeper); // Operation for this keyword
    struct keyWordStruct *next;
}KeyWord;
```

Keeps all keywords in a linked list.

#### All Flags

Flag               | Binary Value | Decimal Value | Description
---------------    | ------------ | ------------- | -----------
LINE_ENDED         | 0b00000001   | 1             | Indicates current is line ended
STRING_EXPECTED    | 0b00000010   | 2             | A String Constant is expected
INT_EXPECTED       | 0b00000100   | 4             | A Int Constant is expected
IDENTIFIER_USE     | 0b00001000   | 8             | A declared identifier is expected
IDENTIFIER_DECLARE | 0b00100000   | 32            | A not declared identifier is expected
BLOCK_EXPECTED     | 0b01000001   | 65            | A Openblock or start of a new line is expected
NOP                | 0b10000000   | 128           | If flag set to NOP lexical analyzer expecting `expectedKeycode`
IDENTIFIER_EXPECTED| 0b00101000   | 40            | Declared or not declared identifier is expected
KEYWORD_EXPECTED   | 0b11000001   | 193           | A keyword is expected
INT_VAL            | 0b00001100   | 12            | Int Constant or declared identifier is expected
OUT_LIST           | 0b00001110   | 14            | A String Constant, Int Constant or declared Identifier is expected

Parser checks flag status with "Bitwise And" or "=="

After a non-keyword word, the lexical analyzer sets the flag to NOP

#### Keycode
```c
typedef enum keycode {bInt, bMove, bAdd, bSub, bOut, bLoop, bOpenBlock, bCloseBlock, bTimes, bNewline, bTo, bEOL, bSeparator,
    bFrom, bNop} Keycode;
```
Keycodes' type is enum and represents Keywords.

#### TokenType

```c
typedef enum tokentype {bKeyword, bIntConstant, bStringConstant, bIdentifier,  bEOF, bError} TokenType;
```

Token type represents current tokens type.
Used by lexical analyzer and parser

#### All Keywords
Keycode           | Keyword | Expected Keycode | Expected keyword | Expected Flag      | Next Flag            | Operation
-------           | ------- | ---------------- | ---------------- | ------------------ | ---------            | --------
bInt              | int     | 11               | .                | LINE_ENDED         | IDENTIFIER_DECLARE   | identifier
bMove             | move    | 10               | to               | LINE_ENDED         | INT_VAL              | move
bAdd              | add     | 10               | to               | LINE_ENDED         | INT_VAL              | add
bSub              | sub     | 13               | from             | LINE_ENDED         | INT_VAL              | sub
bOut              | out     | 12               | ,                | LINE_ENDED         | OUT_LIST             | out
bLoop             | loop    | 8                | times            | LINE_ENDED         | INT_VAL              | NULL
bOpenBlock        | [       | -1               |                  | BLOCK_EXPECTED     | LINE_ENDED           | NULL
bCloseBlock       | ]       | -1               |                  | LINE_ENDED         | LINE_ENDED           | NULL
bTimes            | times   | -1               |                  | NOP                | BLOCK_EXPECTED       | NULL
bNewLine          | newline | 12               | ,                | OUT_LIST           | NOP                  | NULL
bTo               | to      | 11               | .                | NOP                | IDENTIFIER_USE       | NULL
bEOL              | .       | -1               |                  | NOP                | LINE_ENDED           | NULL
bSeparator        | ,       | 12               | ,                | NOP                | OUT_LIST             | NULL
bFrom             | from    | 11               | .                | NOP                | IDENTIFIER_USE       | NULL

* Keycode: Keywords' code.
* Keyword: Keyword itself.
* Expected KeyCode: Next expected keycode.
* Expected Keyword: Next expected keyword.
* Expected Flag: Expected flag for the use of this KeyWord.
* Next Flag: Flag for next expected "word".
* Operation: Function for keyword

Parser always checks flag first.

If `(flag & KEYWORD_EXPECTED)` bitwise operation is `0`,
parser expects a non-keyword word.

Otherwise, the parser expects a keyword according to the expectedKeycode.

Note: If the Expected keycode is bNop, the parser expects the flag to be LINE_ENDED.
Otherwise, the parser expects the flag to be NOP.

#### Keyword Functions

* ```c 
  KeyWord *addKeyWord(char *keyWord, KeyWord *prev, char keycode, char expectedKeyCode, unsigned char flagsForKeyWord,
                    unsigned char flagsForNextKeyWord);
  ```
  * Adds keyword to list.

* ```c 
  KeyWord *createKeyWordLinkedList();
  ```
  * Creates linked list and returns the root of it.

* ```c 
  KeyWord *getKeyWord(char *keyWord, KeyWord *keyWordRoot);
  ```
  * Returns KeyWord pointer using keyWord string(char array). Returns NULL if keyWord not in the list.

* ```c 
  KeyWord *getKeyWordByKeyCode(Keycode keyCode, KeyWord *keyWordRoot);
  ```
  * Same as above. Just uses keyCodes to find KeyWord.

* ```c 
  void freeKeyWordLinkedList(KeyWord *root);
  ```
  * Frees Linked List

* ```c 
  bool isIntConstant(char *str);
  ```
  * Checks given string is an IntConstant or not.

* ```c 
  bool isIdentifier(char *str);
  ```
  * Checks given string is an Identifier or not.

* ```c 
  void identifier(Variable* stack, IdentifierKeeper* identifierKeeper);
  ```
  * Creates identifier.

* ```c 
  void move(Variable* stack, IdentifierKeeper* identifierKeeper);
  ```
  * Moves a value to identifier

* ```c 
  void add(Variable* stack, IdentifierKeeper* identifierKeeper);
  ```
  * Adds a value to identifier

* ```c 
  void sub(Variable* stack, IdentifierKeeper* identifierKeeper);
  ```
  * Subtracts a value to identifier

* ```c 
  void out(Variable* stack, IdentifierKeeper* identifierKeeper);
  ```
  * Prints given values

### LexicalAnalyzer

#### LexicalData struct
```c
typedef struct lexicalDataKeeper{
    FILE* fPtr;
    Word* currWord;
    KeyWord *currKeyword;
    unsigned int currLine;
}LexicalData;
```

Keeps lexical analyzers' data.

#### Functions

* ```c
    TokenType lexicalAnalyzer(LexicalData* data, KeyWord *root);
    ```
  Gets next word in file and determines its TokenType. If it is a Keyword, assigns currKeyword pointer in
  the data(Lexical Data) to that keywords' pointer.

* ```c
    void freeLexicalData(LexicalData* data);
    ```
  Frees given LexicalData.

### Parser

* ```c
    void parser(LexicalData* data);
  ```
  Parses given the file in Lexical data, and executes operations if the syntax is true.

* ```c
    void loop(Variable* stack, IdentifierKeeper* identifierKeeper, LexicalData* data, BlockKeeper* blockKeeper,
   bool isShortHandLoop);
  ```
  Creates a loop.

#### How does it work?

Parser uses 3 variables to check syntax. These are:
* ```c
    unsigned char flag; (Inital value is LINE_ENDED)
    ```

* ```c
    TokenType currToken;
    ```

* ```c
    Keycode expectedKeycode; (Inital value is bNop)
    ```

The parser expects words that suit the current flag. The parser uses "Bitwise AND" and "Equality Operator" to specify the type of the expected word.

If the Current Words' TokenType is different from the expected type or the Current keyword is
different from the expected keyword, the parser throws an error message using the "err" function.

Otherwise, continues to prepare the current operation.

#### How operations executed

If the parser sees no problem with the current word, prepares an operation.
Operation functions kept in Keyword struct. Operation function receives 2 arguments.
These are Variable and IdentifierKeeper.

Parser adds Variables to stack as they come.

At the End of Line, the operation function executes the operation using Variable stack and Identifier keeper.



## General purpose functions
Lexical Analyzer has two header files. These are `fileio.h` and `errhandle.h`
* `str.h` -> Functions about string(strclr)
* `fileio.h` -> Functions about file operations
* `errhandle.h` -> Functions about error handling

### fileio.h

* ```c 
  FILE *openFile(char *path, char *mode);
  ```
  * Opens file at the given path. If it fails, print an error message.

* ```c 
  bool skipIgnoreChars(FILE *fPtr, LineTracker *tracker);
  ```
  * Skips all unimportant characters in given file. Unimportant characters are:
    * White Space (' ')
    * Line break ('\n')
    * Carriage return ('\r')
    * Tab character ('\t'')
  * Returns true if a character skipped.

* ```c 
  bool skipCommentBlocks(FILE *fPtr, LineTracker *tracker);
  ```
  * Skips Comment blocks. Returns true if a comment block skipped

* ```c 
  void seekEOL(FILE *fPtr, LineTracker *tracker);
  ```
  * Seeks to End of Line('.')

* ```c 
  void getWord(char *out, FILE *fPtr, LineTracker *tracker, const int BUFFER_SIZE);
  ```
  * Gets a word in file and writes it to out.
  * There are exceptions at reading. Exceptions are:
    * If first character read is,
      * End Of Line ('.')
      * Separator (',')
      * Open or Close Block ('\[' / '\]')

      only write read character and return.
    * If the first character read is Lexeme String (' \" '), Reads until another Lexeme String. If can't find another Lexeme String, returns.

    * Otherwise, reads until the end of word. (Word termination characters are one of above, White space and COMMENT_OPEN)

### errhandle.h

* ```c 
  void err(char *word, KeyWord *root, char *expectedKeycode, unsigned char *flag, FILE *fPtr, LineTracker *tracker);
  ```
  * This function handles all errors.
  * Prints Expected word, read word, line information about the error.
  * Resets lexical analyzer

* ```c 
  void checkBlocks(BlockKeeper *blockKeeper);
  ```
  * Checks for blocks which is/are left open.
  * If there is one, Prints information about left open block/s

### str.h

* ```c 
  void strclr(char *str, const int BUFFER_SIZE);
  ```
  * Clears given String(char array). BUFFER_SIZE is the size of the char array.
  
