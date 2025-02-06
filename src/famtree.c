//Lab 2: Family Tree
//Madelyn Gross

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
    int visited;
    int unprintedParents;//counting how many parents get assigned so we know when they can be printed
} Person;

char *createName(IS is){
    if(is->NF <= 1){
        return strdup("");
    }

    int totalLength = 0;
    //we do not want the first one because it will include relation
    for(int i = 1; i < is->NF; i++){
        totalLength += strlen(is->fields[i]) + 1;//length of the field and a space/empty character per field
    }

    char *name = malloc(totalLength);//allocate length of name
    if(!name){
        return NULL;
    }

    name[0]='\0';

    for(int i = 1; i < is->NF; i++){
        strcat(name, is->fields[i]);//concatonate each of the fields after the first
        if(i < (is->NF - 1)){
            strcat(name, " " );
        }
    }

    return name;
}

//using dfs to find if there is a cycle in the tree
int findCycle(Person *person){
    if(person->visited == 1){
        fprintf(stderr,"Bad input -- cycle in specification\n");
        return 1;
    }
    person->visited = 1;

    Dllist kid;

    //going through all of the persons children
    dll_traverse(kid, person->children){
        Person *child = (Person *) kid->val.v;

        if(findCycle(child)){
            return 1;
        }
    }
    //resetting visited flag so that next run through can happen correctly
    person->visited = 0;
    return 0;
}

int main()
{   
    //part of new libraries will allow for easy inputs
    IS is;
    //and easy insertion into trees
    JRB people, temp;

    //initialize the helper containers
    is = new_inputstruct(NULL);
    people = make_jrb();

    //just make sure the file can be opened
    if(is == NULL) {
        fprintf(stderr, "file cannot be opened");
        return 1;
    }
    //this is the person who is being referred to established by PERSON being the key word
    Person *currentPerson;

    //reading each line in the file
    while(get_line(is) >= 0){
        //make sure there is something in the line we dont care otherwise
        if(is->NF > 0){
            //the person whose name is in the current line
            Person *linePerson;
            //for any line where a person can be found, SEX will never establish a new person
            if(strcmp(is->fields[0], "SEX") != 0) {
                char *name = createName(is);
                //checking if person already exists within the tree
                temp = jrb_find_str(people, name);
                //if they do not already exist create them
                if(temp == NULL){
                    linePerson = (Person *)malloc(sizeof(Person));
                    linePerson->name = name;
                    linePerson->sex = "";
                    linePerson->mother = NULL;
                    linePerson->father = NULL;
                    linePerson->visited = 0;
                    linePerson->children = new_dllist();
                    linePerson->unprintedParents = 0;
                    
                    jrb_insert_str(people, linePerson->name, new_jval_v((void *) linePerson));
                }
                //set line person to the person in the tree
                linePerson = (Person *) jrb_find_str(people, name)->val.v;
            }
            //if they are null, but this would mean something failed or did not work
            if(linePerson == NULL) fprintf(stderr, "idk why but maybe this is the problem");
            //this sets this person to the current person
            if(strcmp(is->fields[0], "PERSON") == 0){
                currentPerson = linePerson;
            } 
            //do we need to set the father and sex? do they match?
            //assign relationships, check for errors in assignment do this for all possible 
            //inputs, what do these relationships mean for the peoples sex?
            else if(strcmp(is->fields[0], "FATHER") == 0){
                if(currentPerson->father == NULL){
                    currentPerson->father = linePerson;
                    dll_append(linePerson->children,new_jval_v((void *) currentPerson));
                    currentPerson->unprintedParents++;
                } else if(strcmp(currentPerson->father->name, linePerson->name) != 0){
                    fprintf(stderr, "Bad input -- child with two fathers on line %i\n", is->line);
                    return 1;
                }
                if(strcmp(linePerson->sex, "") == 0){
                    linePerson->sex = strdup("M");
                } else if(strcmp(linePerson->sex, "M") != 0){
                    fprintf(stderr, "Bad input - sex mismatch on line %i\n", is->line);
                    return 1;
                }
            } else if(strcmp(is->fields[0], "MOTHER") == 0){
                if(currentPerson->mother == NULL){
                    currentPerson->mother = linePerson;
                    dll_append(linePerson->children,new_jval_v((void *) currentPerson));
                    currentPerson->unprintedParents++;
                } else if(strcmp(currentPerson->mother->name, linePerson->name) != 0){
                    fprintf(stderr, "Bad input -- child with two mothers on line %i\n", is->line);
                    return 1;
                }
                if(strcmp(linePerson->sex, "") == 0){
                    linePerson->sex = strdup("F");
                } else if(strcmp(linePerson->sex, "F") != 0){
                    fprintf(stderr, "Bad input - sex mismatch on line %i\n", is->line);
                    return 1;
                }
            } else if(strcmp(is->fields[0], "MOTHER_OF") == 0){
                if(linePerson->mother == NULL){
                    linePerson->mother = currentPerson;
                    dll_append(currentPerson->children,new_jval_v((void *) linePerson));
                    linePerson->unprintedParents++;
                } else if(strcmp(linePerson->mother->name, currentPerson->name) != 0){
                    fprintf(stderr, "Bad input -- child with two mothers on line %i\n", is->line);
                    return 1;
                }
                if(strcmp(currentPerson->sex, "") == 0){
                    currentPerson->sex = strdup("F");
                } else if(strcmp(currentPerson->sex, "F") != 0){
                    fprintf(stderr, "Bad input - sex mismatch on line %i\n", is->line);
                    return 1;
                }
            } else if(strcmp(is->fields[0], "FATHER_OF") == 0){
                if(linePerson->father == NULL){
                    linePerson->father = currentPerson;
                    dll_append(currentPerson->children,new_jval_v((void *) linePerson));
                    linePerson->unprintedParents++;
                } else if(strcmp(linePerson->father->name, currentPerson->name) != 0){
                fprintf(stderr, "Bad input -- child with two fathers on line %i\n", is->line);
                return 1;
                }
                if(strcmp(currentPerson->sex, "") == 0){
                    currentPerson->sex = strdup("M");
                } else if(strcmp(currentPerson->sex, "M") != 0){
                    fprintf(stderr, "Bad input - sex mismatch on line %i\n", is->line);
                    return 1;
                }
            } else if(strcmp(is->fields[0], "SEX") == 0){
                if(strcmp(currentPerson->sex, "") == 0){
                    currentPerson->sex = strdup(is->fields[1]);
                } else if(strcmp(currentPerson->sex, is->fields[1]) != 0){
                    fprintf(stderr, "Bad input - sex mismatch on line %i\n", is->line);
                    return 1;
                }
            }
        }
    }

    //This is how I will implement topological sort, this is the list of people who
    //are allowed to be printed
    Dllist peopleToPrint = new_dllist();
    Dllist tempPerson;

    jrb_traverse(temp, people){
        currentPerson = (Person *) temp->val.v;
        //finding the cycles 
        if(findCycle(currentPerson)){
            return 1;
        }
        //if none of their parents have been told to us they are okay for us to begin printing
        if(currentPerson->unprintedParents == 0){
            dll_append(peopleToPrint, new_jval_v((void *) currentPerson));
        }
    }

    //now I am going to traverse the linked list 
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
        //going through the list of children if they have them
        if (!dll_empty(person->children)) {
            printf("\tChildren: \n");
            Dllist node;
            dll_traverse(node, person->children) {
                Person *child = (Person *) node->val.v;
                if(child->name){
                    printf("\t  %s\n", child->name);
                }else{
                    printf("houston we have a problem");
                }
                //acknowledging that the parent has been printed and if that brings the
                //unprinted parents to 0 adding them to the list that can be printed
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

    //freeing all of the things I allocated memory for
    jettison_inputstruct(is);

    jrb_traverse(temp, people){
        Person *person = (Person *) temp->val.v;

        if(person){
            free(person->name);
            if(strcmp(person->sex, "") != 0){
                free(person->sex);
            }

            free_dllist(person->children);

            free(person);
        }
    }

    jrb_free_tree(people);

    return 0;
}
