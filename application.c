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

int checkIfNameExist(char production[], char name[]) {
    if (strlen(name) == 2) {
        for (int i = 0; i < strlen(production) - 1; ++i) {
            if (name[0] == production[i] && name[1] == production[i + 1])
                return i + 2;
        }
    }
    else {
        for (int i = 0; i < strlen(production); ++i) {
            if (name[0] == production[i] && production[i + 1] != '\'')
                return i + 1;
        }
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

Rules* followForOneRule(Rules* p, Rules* p2, Rules* h) {
    if (p->followCalculator == 0) {
        p->followCalculator = 1;
        while (p2 != NULL) {
            int max = p2->count;
            for (int i = 0; i < max; ++i) {
                int d = checkIfNameExist(p2->production[i], p->name);
                if (d == 0) {
                    continue;
                }
                if (p2->production[i][d] == '\0') {
                    if (strcmp(p->name, p2->name) != 0) {
                        if (!isupper(p2->production[i][d - 1]) && p2->production[i][d - 1] != '\'') {
                            p->follow[p->followCounter] = p2->production[i][d - 1];
                            p->followCounter++;
                        }
                        else {
                            Rules* temp = followForOneRule(p2, h, h);
                            if (p->follow[0] == '\0')
                                strcpy(p->follow, temp->follow);
                            else
                                strcat(p->follow, temp->follow);
                            p->followCounter = p->followCounter + strlen(temp->follow);
                        }
                    }
                }
                else {
                    while (d < strlen(p2->production[i])) {
                        if (isupper(p2->production[i][d])) {
                            Rules* temp;
                            if (p2->production[i][d + 1] == '\'') {
                                char x[3];
                                x[0] = p2->production[i][d];
                                x[1] = '\'';
                                temp = nameToRule(h, x);
                                d = d + 2;
                            }
                            else {
                                temp = getRuleByName(h, p2->production[i][d]);
                                d++;
                            }
                            if (epsilonExists(temp->first)) {
                                strcat(p->follow, temp->first);
                                strcpy(p->follow, removeEpsilon(p->follow));
                                p->followCounter = p->followCounter + temp->firstCounter - 1;
                                if (p2->production[i][d] == '\0' && strcmp(temp->name, p2->name) != 0) {
                                    strcat(p->follow, p2->follow);
                                    p->followCounter = p->followCounter + strlen(p2->follow);
                                }
                            }
                            else {
                                strcat(p->follow, temp->first);
                                p->followCounter = p->followCounter + temp->firstCounter;
                                break;
                            }
                        }
                        else {
                            p->follow[p->followCounter] = p2->production[i][d];
                            p->followCounter++;
                            p->follow[p->followCounter] = '\0';
                            break;
                        }
                    }
                }
            }
            p2 = p2->next;
        }
        strcpy(p->follow, removeDuplicated(p->follow));
        p->followCounter = strlen(p->follow);
        return p;
    }
    return p;
}

Rules* follow(Rules* p) {
    Rules* iterator = p;
    while (iterator != NULL) {
        iterator = followForOneRule(iterator, p, p);
        iterator = iterator->next;
    }
    return p;
}

Rules* nameToRule(Rules* p, char* name) {
    while (p != NULL) {
        if (strcmp(name, p->name) == 0) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}


void printFollow(Rules* p) {
    while (p != NULL) {
        printf("follow(%s) = {", p->name);
        for (int i = 0; i < p->followCounter; ++i) {
            if (i == p->followCounter - 1) {
                printf("%c", p->follow[i]);
            }
            else {
                printf("%c,", p->follow[i]);
            }
        }
        printf("}\n");
        p = p->next;
    }
}

int find(Rules* p, Rules* h, char c) {
    for (int i = 0; i < p->count; ++i) {
        int j = 0;
        while (j < strlen(p->production[i])) {
            if (p->production[i][j] == c){
                return i;
            }
            if (isupper(p->production[i][j])){
                int k;
                if (p->production[i][j+1] != '\''){
                    k = find(getRuleByName(h, p->production[i][j]), h, c);
                    ++j;
                } else {
                    char x[3];
                    x[0] = p->production[i][j];
                    x[1] = '\'';
                    k  = find(nameToRule(h,x),h,c);
                    j = j+2;
                }
                if (k == 2) {
                    continue;
                } else {
                    return i;
                }
            } else if(p->production[i][j] == c){
                return 1;
            } else if(p->production[i][j] == '&'){
                return 2;
            }
            break;
        }
    }
    return 0;
}

void insertLL1(LL1 ** table,LL1 * temp){
    temp->next = NULL;
    if (*table == NULL) {
        *table = temp;
    } else {
        LL1 * x = (*table);
        while (x->next != NULL) {
            x = x->next;
        }
        x->next = temp;
    }
}
LL1 * generateLL1Table(Rules * p, Rules * h){
    LL1 * table = NULL;
    while (p!=NULL) {
        for (int i = 0; i < p->firstCounter; ++i) {
            if (p->first[i] == '&') {
                for (int j = 0; j < p->followCounter; ++j) {
                    LL1 * temp = malloc(sizeof(LL1));
                    temp->result[0] ='&';
                    strcpy(temp->nonTerminal, p->name);
                    temp->terminal = p->follow[j];
                    insertLL1(&table,temp);
                }
            } else {
                int k= find(p,h,p->first[i]);
                LL1 * temp = malloc(sizeof(LL1));
                strcpy(temp->result ,p->production[k]);
                strcpy(temp->nonTerminal, p->name);
                temp->terminal = p->first[i];
                insertLL1(&table,temp);
            }
        }
        p = p->next;
    }
    return table;
}

void printLL1(LL1 * l) {
    while (l!= NULL) {
        printf("( %s,%c ) -> %s \n", l->nonTerminal, l->terminal, l->result);
        l = l->next;
    }
}

int main()
{

    return 0;
}