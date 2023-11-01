#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct LL1 LL1;
typedef struct Rules Rules;
typedef struct Stack Stack;

static const int NUMBER_OF_RULES = 40;
static const int WORD = 100;

struct LL1
{
    char nonTerminal[3];
    char terminal;
    char result[5];
    struct LL1 *next;
};

struct Rules
{
    char name[3];
    char production[10][10];
    char follow[10];
    char first[10];
    int firstCounter;
    int followCounter;
    int count;
    int firstCalculator;
    int followCalculator;
    struct Rules *next;
};

struct Stack
{
    char value[3];
    struct Stack *next;
};


int main()
{

    return 0;
}