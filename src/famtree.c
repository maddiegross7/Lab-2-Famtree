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

int doesPersonExist(){

}

int main(int argc, char const *argv[])
{
    IS is;
    JRB people, temp;

    is = new_inputstruct(NULL);
    people = make_jrb();

    if(is == NULL) {
        perror("file cannot be opened");
    }

    printf("hello is is not null\n");

    Person *currentPerson;

    while(get_line(is) >= 0){
        //printf("Number of Fields: %i", is->NF);
        // printf("%d: %s", is->line, is->text1);
        // if (is->text1[0] != '\n'|| is->text1[0] != '\0') {
        //     printf("Field 1: %s, Field 2: %s\n\n", is->fields[0], is->fields[1]);
        // }

        if(strcmp(is->fields[0],"SEX")){
            if(jrb_find_str(people, is->fields[1]) == NULL){
                printf("%s has not been created yet\n", is->fields[1]);
            }
        }

    }

    return 0;
}
