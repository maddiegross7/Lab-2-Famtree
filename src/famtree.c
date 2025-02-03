#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fields.h"
#include "jval.h"
#include "dllist.h"
#include "jrb.h"

typedef struct person{
    char *name;
    char sex;
    struct person *father;
    struct person *mother;
    struct person **children;

    int visited;
} Person;

int main(int argc, char const *argv[])
{
    IS is;
    
    is = new_inputstruct(NULL);
    if(is == NULL) {
        perror("file cannot be opened");
    }

    printf("hello is is not null\n");
    
    while(get_line(is) >= 0){
        for(int i=0; i < is->NF; i++){
            printf("%d: %s\n", is->line, is->fields[i]);
        }
    }

    return 0;
}
