#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fields.h"
#include "jval.h"
#include "dllist.h"
#include "jrb.h"

//./bin/famtree < data/fam1.txt

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
        //printf("%d: %s", is->line, is->text1);
        // if (is->text1[0] != '\n'|| is->text1[0] != '\0') {
        printf("Field 1: %s, Field 2: %s\n\n", is->fields[0], is->fields[1]);
        // }
        
        if(strcmp(is->fields[0],"SEX")!=0){
            // if (temp == NULL) {
            //     printf("temp is NULL\n");
            // } else {
            //     printf("Before seek: %s\n", temp->val);
            // }
            // //printf("Before seek: %s", temp->val);
            temp = jrb_find_str(people, is->fields[1]);
            // //printf("After seek: %s", temp->val);
            // if (temp == NULL) {
            //     printf("temp is NULL\n");
            // } else {
            //     printf("After seek: %s\n", temp->val);
            // }

            if(temp == NULL){
                printf("%s has not been created yet\n", is->fields[1]);

                Person *newPerson = (Person *)malloc(sizeof(Person));

                newPerson->name = strdup(is->fields[1]);
                
                //printf("Field 1 hellllloooooooo: %s\n", is->fields[1]);
                jrb_insert_str(people, newPerson->name, new_jval_v((void *) newPerson));

                // if(strcmp(is->fields[0],"PERSON")==0)
                //currentPerson = newPerson;
            }
        }
    }

    jrb_traverse(temp, people) {
        currentPerson = (Person *) temp->val.v;
        printf("%-40s\n", currentPerson->name);
    }

    return 0;
}
