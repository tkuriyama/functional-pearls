#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const int BIRTHDAYSIZE = 12;
const int NAMESIZE = 40;
  
struct record {
  char birthday[BIRTHDAYSIZE];
  char name[NAMESIZE];
  struct record *father;
  struct record *mother;
};

/* safe malloc */

void *emalloc(size_t sz) {
   void *p;
   if((p = malloc(sz)) == NULL) abort();
   return p;
}

/* creating and destroying ancestry tree */

// parse a csv string line into static array
char **parse(char *line, char *delim) {
  // strip trailing newline
  int len = strlen(line);
  if (line[len-1] == '\n')
    line[len-1] = '\0';
  
  char **ap;
  static char *argv[4];
  for (ap = argv; (*ap = strsep(&line, delim)) != NULL;) 
    if ((**ap != '\0') && (++ap >= &argv[4]))
      break;    
  return argv;
}

void populate(struct record *person, char *name) {
  strcpy(person->name, name);
  //strcpy(person->birthday, "unknown");
  strcpy(person->birthday, "unknown");
  person->father = NULL;
  person->mother = NULL;
}

void insert(char **fields, struct record *persons) {
  // Found tree root or correct record
  if ((strcmp(persons->name, "") == 0) ||
      (strcmp(persons->name, fields[1]) == 0)) {
    struct record *father, *mother;
    father = emalloc(sizeof(struct record));
    mother = emalloc(sizeof(struct record));
    populate(father, fields[2]);
    populate(mother, fields[3]);
      
    strcpy(persons->name, fields[1]);
    strcpy(persons->birthday, fields[0]);
    persons->father = father;
    persons->mother = mother;
  }
  // Keep Looking
  else {
    if (persons->father != NULL) insert(fields, persons->father);
    if (persons->mother != NULL) insert(fields, persons->mother);
  }
  return;
}

void freeTree(struct record *persons) {
  if (persons->father != NULL) freeTree(persons->father);
  if (persons->mother != NULL) freeTree(persons->mother);
  free(persons);
  return;
}

/* Find Anceststry */

void printTree(struct record *persons) {
  if (persons->father != NULL) printTree(persons->father);
  if (persons->father != NULL) printTree(persons->mother);  
  printf("Record: %s, %s\n", persons->name, persons->birthday);
  return;
}

void printAncestor(char *name, char *birthday, int level, int paternal) {
  if (level == 0) {
    printf("Self: %s, %s\n", name, birthday);
  }
  else {
    if (level > 2)
      for (int i = 2; i < level; ++i) printf("Great ");
    if (level > 1) 
      printf("Grand ");

    const char * parent = (paternal) ? "Father" : "Mother";
    printf("%s: %s, %s\n", parent, name, birthday);
  } 
}

void showAncestry(struct record *persons, char *name, int level, int paternal) { 

  if(strcmp(persons->name, name) == 0 || level > 0) {
    printAncestor(persons->name, persons->birthday, level, paternal);
    if (persons->father != NULL)
      showAncestry(persons->father, persons->father->name, level+1, 1);
    if (persons->mother != NULL)
      showAncestry(persons->mother, persons->mother->name, level+1, 0);    
  }
  else {
    if (persons->father != NULL) showAncestry(persons->father, name, 0, 1);
    if (persons->mother != NULL) showAncestry(persons->mother, name, 0, 0);    
  }
  return;
}

int main(int argc, char **argv) {
  struct record *persons;
  persons = emalloc(sizeof(struct record));
  strcpy(persons->name, "");

  FILE * f;
  char *line, **fields;
  size_t linecap = 0;  
  f = fopen("ancestors.csv", "r");
  
  while(getline(&line, &linecap, f) != -1) {
    fields = parse(line, ",");
    insert(fields, persons);
  }
  
  free(line);
  fclose(f);

  // printTree(persons);
  if (argc > 1) 
    showAncestry(persons, argv[1], 0, 0);

  freeTree(persons);
  return 0;
}


