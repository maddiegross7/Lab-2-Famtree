#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fields.h"
#include "jval.h"
#include "dllist.h"
#include "jrb.h"

//./bin/famtree < data/fam1.txt

typedef struct person {
    char *name;
    char *sex;
    struct person *father;
    struct person *mother;
    Dllist children;
    int childCount;
    int visited;
    int unprintedParents;
} Person;

char *createName(IS is){
    if(is->NF <= 1){
        return strdup("");
    }

    int totalLength = 0;
    //we do not want the first one
    for(int i = 1; i < is->NF; i++){
        totalLength += strlen(is->fields[i]) + 1;
    }

    char *name = malloc(totalLength);
    if(!name){
        return NULL;
    }

    name[0]='\0';

    for(int i = 1; i < is->NF; i++){
        strcat(name, is->fields[i]);
        if(i < (is->NF - 1)){
            strcat(name, " " );
        }
    }

    return name;
}

int findCycle(Person *person){
    if(person->visited == 1){
        fprintf(stderr,"Bad input -- cycle in specification\n");
        return 1;
    }

    person->visited = 1;

    Dllist kid;

    dll_traverse(kid, person->children){
        Person *child = (Person *) kid->val.v;

        if(findCycle(child)){
            return 1;
        }
    }
    person->visited = 0;
    return 0;

}

int main(int argc, char const *argv[])
{
    IS is;
    JRB people, temp;

    is = new_inputstruct(NULL);
    people = make_jrb();

    if(is == NULL) {
        fprintf(stderr, "file cannot be opened");
        return 1;
    }

    Person *currentPerson;

    while(get_line(is) >= 0){
        if(is->NF > 0){
            Person *linePerson;
        //printf("text 1: %s text 2: %s ", is->text1, is->text2);
        if(strcmp(is->fields[0], "SEX") != 0) {
            //printf("hello");
            char *name = createName(is);
            //printf("hola");
            temp = jrb_find_str(people, name);
            if(temp == NULL){
                linePerson = (Person *)malloc(sizeof(Person));
                linePerson->name = name;
                linePerson->sex = "";
                linePerson->mother = NULL;
                linePerson->father = NULL;
                linePerson->visited = 0;
                linePerson->childCount = 0;
                linePerson->children = new_dllist();
                linePerson->unprintedParents = 0;
                
                jrb_insert_str(people, linePerson->name, new_jval_v((void *) linePerson));
            }
            linePerson = (Person *) jrb_find_str(people, name)->val.v;
        }
        if(linePerson == NULL) fprintf(stderr, "idk why but maybe this is the problem");
        if(strcmp(is->fields[0], "PERSON") == 0){
            currentPerson = linePerson;
            //printf("hello %s is the current player\n\n", currentPerson->name);
            //printf("why here");
        } else if(strcmp(is->fields[0], "FATHER") == 0){
            if(currentPerson->father == NULL){
                currentPerson->father = linePerson;
                linePerson->childCount++;
                dll_append(linePerson->children,new_jval_v((void *) currentPerson));
                currentPerson->unprintedParents++;
            } else if(strcmp(currentPerson->father->name, linePerson->name) != 0){
                fprintf(stderr, "Bad input -- child with two fathers on line %i\n", is->line);
                return 1;
            }
            if(linePerson->sex == ""){
                linePerson->sex = strdup("M");
            } else if(strcmp(linePerson->sex, "M") != 0){
                fprintf(stderr, "Bad input - sex mismatch on line %i\n", is->line);
                return 1;
            }
            
        } else if(strcmp(is->fields[0], "MOTHER") == 0){
            if(currentPerson->mother == NULL){
                currentPerson->mother = linePerson;
                linePerson->childCount++;
                dll_append(linePerson->children,new_jval_v((void *) currentPerson));
                currentPerson->unprintedParents++;
            } else if(strcmp(currentPerson->mother->name, linePerson->name) != 0){
                fprintf(stderr, "Bad input -- child with two mothers on line %i\n", is->line);
                return 1;
            }
            if(linePerson->sex == ""){
                linePerson->sex = strdup("F");
            } else if(strcmp(linePerson->sex, "F") != 0){
                fprintf(stderr, "Bad input - sex mismatch on line %i\n", is->line);
                return 1;
            }
            
        } else if(strcmp(is->fields[0], "MOTHER_OF") == 0){
            if(linePerson->mother == NULL){
                linePerson->mother = currentPerson;
                currentPerson->childCount++;
                dll_append(currentPerson->children,new_jval_v((void *) linePerson));
                linePerson->unprintedParents++;
            } else if(strcmp(linePerson->mother->name, currentPerson->name) != 0){
                fprintf(stderr, "Bad input -- child with two mothers on line %i\n", is->line);
                return 1;
            }
            if(currentPerson->sex == ""){
                currentPerson->sex = strdup("F");
            } else if(strcmp(currentPerson->sex, "F") != 0){
                fprintf(stderr, "Bad input - sex mismatch on line %i\n", is->line);
                return 1;
            }
        } else if(strcmp(is->fields[0], "FATHER_OF") == 0){
            if(linePerson->father == NULL){
                linePerson->father = currentPerson;
                currentPerson->childCount++;
                //printf("setting %s's father to %s\n", linePerson->name, currentPerson->name);
                dll_append(currentPerson->children,new_jval_v((void *) linePerson));
                linePerson->unprintedParents++;
            } else if(strcmp(linePerson->father->name, currentPerson->name) != 0){
               fprintf(stderr, "Bad input -- child with two fathers on line %i\n", is->line);
               return 1;
            }
            if(currentPerson->sex == ""){
                currentPerson->sex = strdup("M");
            } else if(strcmp(currentPerson->sex, "M") != 0){
                fprintf(stderr, "Bad input - sex mismatch on line %i\n", is->line);
                return 1;
            }
        } else if(strcmp(is->fields[0], "SEX") == 0){
            if(currentPerson->sex == ""){
                currentPerson->sex = strdup(is->fields[1]);
            } else if(strcmp(currentPerson->sex, is->fields[1]) != 0){
                fprintf(stderr, "Bad input - sex mismatch on line %i\n", is->line);
                return 1;
            }
        }
        }
        
    }

    Dllist peopleToPrint = new_dllist();
    Dllist tempPerson;

    jrb_traverse(temp, people){
        currentPerson = (Person *) temp->val.v;

        if(findCycle(currentPerson)){
            return 1;
        }

        if(currentPerson->unprintedParents == 0){
            //printf("adding %s to people to print", currentPerson->name);
            dll_append(peopleToPrint, new_jval_v((void *) currentPerson));
        }
    }


    dll_traverse(tempPerson, peopleToPrint){
        Person *person = (Person *) tempPerson->val.v;

        printf("%s\n", person->name);

        if(strcmp(person->sex, "M")==0){
            printf("\tSex: Male\n");
        }else if(strcmp(person->sex, "F")==0){
            printf("\tSex: Female\n");
        }else{
            printf("\tSex: Unknown\n");
        }

        if(person->father){
            printf("\tFather: %s\n", person->father->name);
        }else{
            printf("\tFather: Unknown\n");
        }

        if(person->mother){
            printf("\tMother: %s\n", person->mother->name);
        }else{
            printf("\tMother: Unknown\n");
        }

        if (!dll_empty(person->children)) {
            printf("\tChildren: \n");
            Dllist node;
            dll_traverse(node, person->children) {
                //printf("hello");
                Person *child = (Person *) node->val.v;
                if(child->name){
                    printf("\t  %s\n", child->name);
                }else{
                    printf("houston we have a problem");
                }
                 
                child->unprintedParents--;
                if(child->unprintedParents == 0){
                    dll_append(peopleToPrint, new_jval_v((void *) child));
                }
            }
        } else {
            printf("\tChildren: None\n");
        }

        printf("\n");
    }



    // jrb_traverse(temp, people) {
    //     currentPerson = (Person *) temp->val.v;
    //     printf("Name: %-40s\n", currentPerson->name);
        
    //     // Print Sex (handle NULL)
    //     printf("Sex: %s\n", currentPerson->sex ? currentPerson->sex : "Unknown");

    //     // Print Father (handle NULL)
    //     printf("Father: %s\n", currentPerson->father ? currentPerson->father->name : "None");

    //     // Print Mother (handle NULL)
    //     printf("Mother: %s\n", currentPerson->mother ? currentPerson->mother->name : "None");

    //     // Print Children (traverse Dllist)
    //     if (!dll_empty(currentPerson->children)) {
    //         printf("Children: ");
    //         Dllist node;
    //         dll_traverse(node, currentPerson->children) {
    //             Person *child = (Person *) node->val.v;
    //             printf("%s%s", child->name, (node->flink != currentPerson->children) ? ", " : ""); 
    //         }
    //         printf("\n");
    //     } else {
    //         printf("Children: None\n");
    //     }

    //     // Print visited status
    //     printf("Visited: %d\n", currentPerson->visited);
        
    //     printf("--------------------------\n"); // Separator for readability
    // }


    return 0;
}
