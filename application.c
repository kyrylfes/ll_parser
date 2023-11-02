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

void insertDefaultValues(struct Rules ** p, struct Rules * temp){
    if (*p == NULL) {
        temp->next = NULL;
        temp->firstCounter = 0;
        temp->followCounter = 0;
        temp->firstCalculator = 0;
        temp->followCalculator = 0;
        temp->first[0] = '\0';
        temp->follow[0] = '\0';
        *p = temp;
    } else {
        struct Rules * x = (*p);
        while(x->next != NULL){
            x = x->next;
        }
        x->next = temp;
        temp->firstCounter = 0;
        temp->firstCalculator = 0;
        temp->followCalculator =0;
        temp->followCounter = 0;
        temp->first[0] = '\0';
        temp->follow[0] = '\0';
        temp->next = NULL;
    }
}

char * substring(const char * string, int begin, int num){
    char * res;
    int j = 0;
    res = malloc((num)*sizeof(char));
    for (int i = begin; i < begin+num; ++i) {
        res[j] = string[i];
        j++;
    }
    res[num] = '\0';
    return res;
}

void inputGrammar(struct Rules ** p) {
    char *allRules = malloc(NUMBER_OF_RULES * sizeof(char));
    char *token;
    struct Rules *temp;
    int k = 0;
    while (1) {
        scanf("%s", allRules);
        // is used to finished rules input
        if (strcmp(allRules, ".") == 0) {
            return;
        }

        temp = malloc(sizeof(Rules));
        temp->name[0] = allRules[0];
        temp->name[1] = '\0';

        char *help = substring(allRules, 3, strlen(allRules) - 3);
        token = strtok(help, "|");

        while (token != NULL) {
            strcpy(temp->production[k], token);
            int len = strlen(temp->production[k]);
            temp->production[k][len] = '\0';
            ++k;
            token = strtok(NULL, "|");
        }
        temp->count = k;
        k = 0;

        if (*p == NULL) {
            temp->next = NULL;
            temp->firstCalculator = 0;
            temp->followCalculator = 0;
            *p = temp;
        } else {
            Rules *x = (*p);
            while (x->next != NULL) {
                x = x->next;
            }
            x->next = temp;
            temp->firstCalculator = 0;
            temp->followCalculator = 0;
            temp->next = NULL;
        }
    }
}

int checkForRecursion(Rules *p) {
        for (int i = 0; i < p->count; ++i) {
            if (p->name[0] == p->production[i][0]) {
                return 1;
            }
        }
        return 0;
    }

Rules * removeRecursion(Rules * p) {
    Rules * no_rec = NULL;
    Rules * iterator = p;
    while (iterator != NULL) {
        if (checkForRecursion(iterator)) {
            Rules * temp = malloc(sizeof(Rules));
            Rules * temp2 = malloc(sizeof(Rules));

            strcpy(temp->name,iterator->name);
            strcpy(temp2->name,iterator->name);
            strcat(temp2->name,"'");

            int k = 0;
            int l = 0;
            for (int i = 0; i < iterator->count; ++i) {
                if (iterator->production[i][0] != iterator->name[0]) {
                    strcpy(temp->production[k],iterator->production[i]);
                    strcat(temp->production[k],temp2->name);
                    ++k;
                } else {
                    char* help = substring(iterator->production[i],1,strlen(iterator->production[i]));
                    strcpy(temp2->production[l],help);
                    strcat(temp2->production[l],temp2->name);
                    ++l;
                }
            }
            if (k == 0) {
                strcpy(temp->production[0],temp2->name);
                k++;
            }
            temp->count = k;
            strcpy(temp2->production[l],"&");
            temp2->count = l+1;
            insertDefaultValues(&no_rec,temp);
            insertDefaultValues(&no_rec,temp2);
        } else {
            Rules * temp = malloc(sizeof(Rules));
            *temp = * iterator;
            insertDefaultValues(&no_rec,temp);
        }
        iterator = iterator->next;
    }
    return no_rec;
}

char *removeDuplicated(char table[]) {
    char *temp;
    int k = 0;
    temp = malloc(strlen(table));
    for (int i = 0; i < strlen(table); ++i) {
        int exist = 0;
        for (int j = i + 1; j < strlen(table); ++j) {
            if (table[i] == table[j]) {
                exist = 1;
            }
        }
        if (exist == 0) {
            temp[k] = table[i];
            ++k;
        }
    }
    temp[k] = '\0';
    return temp;
}

Rules * getRuleByName(Rules * p, char name){
    while (p != NULL) {
        if (p->name[0] == name && p->name[1] != '\'') {
            return p;
        }
        p = p->next;
    }
    printf("Rule %c does not exist",name);
    exit(0);
}

char* removeEpsilon(char * first){
    int l = 0;
    for (int i = 0; i < strlen(first); ++i) {
        if (first[i] == '&') {
            l++;
        }
    }
    char * temp = malloc(strlen(first));
    l = 0;
    for (int i = 0; i < strlen(first); ++i) {
        if (first[i] != '&') {
            temp[l] = first[i];
            l++;
        }
    }
    temp[l] = '\0';
    return temp;
}

int epsilonExists(char first[]){
    for (int i = 0; i < strlen(first); ++i) {
        if (first[i] == '&') {
            return 1;
        }
    }
    return 0;
}

int checkIfExist(char first[], char pr){
    int i = 0;
    while (i < strlen(first)) {
        if (first[i] == pr) {
            return 1;
        }
        i++;
    }
    return 0;
}

Rules* firstForOneRule(Rules * p, Rules * p2) {
    if (p->firstCalculator == 0) {
        int k =0;
        int max = p->count;
        for (int i = 0; i < max; ++i) {
            int l =0;
            if (isupper(p->production[i][l])) {
                Rules * x = getRuleByName(p2, p->production[i][l]);
                Rules * temp = firstForOneRule(x, p2);
                strcat(p->first,temp->first);
                while(epsilonExists(temp->first) && isupper(p->production[i][l + 1])) {
                    l++;
                    Rules * y = getRuleByName(p2, p->production[i][l]);
                    Rules * temp2 = firstForOneRule(y, p2);
                    strcat(p->first,temp2->first);
                }
                strcpy(p->first,removeEpsilon(p->first));
                strcpy(p->first,removeDuplicated(p->first));
            } else {
                if (!checkIfExist(p->first,p->production[i][0])) {
                    p->first[k] = p->production[i][0];
                    ++k;
                    p->first[k] = '\0';
                }
            }
        }
        p->firstCounter = strlen(p->first);
        p->firstCalculator =1;
        return p;
    }
    return p;
}

Rules* first(Rules* p ) {
    Rules * iterator = p;
    while(iterator!=NULL){
        iterator = firstForOneRule(iterator,p);
        iterator = iterator->next;
    }
    return p;
}

void printFirst(Rules* p){
    while (p != NULL) {
        printf("first(%s) = {",p->name);
        for (int i = 0; i < p->firstCounter; ++i) {
            if (i == p->firstCounter - 1) {
                printf("%c",p->first[i]);
            } else {
                printf("%c,",p->first[i]);
            }
        }
        printf("}\n");
        p = p->next;
    }

}

int main()
{

    return 0;
}