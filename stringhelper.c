#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define BUFFSIZE MAX_CANON

char** stringToArray(char* input);
void destroyStrArray(char** argv);
void readInput(char* buf);

/*char** stringToArray(char* input){
  char buf[BUFFSIZE];
wordexp_t p;
char **w;
char **result = malloc(PATH_MAX*sizeof(char*));
int i;
int len;
  strcpy(buf, input);
  char* t = strtok(buf, " ");
  int count = 1;
  while(strtok(NULL, " "))
    count++;
  strcpy(buf, input);
  t = strtok(buf, " ");
  count = 0;
  while(t){
	wordexp(t, &p, 0);
	w = p.we_wordv;
	for (i = 0; i < p.we_wordc; i++){
	    len = strlen(w[i]);
   	    result[count] = malloc(sizeof(char)*(len+1));
	    strcpy(result[count], w[i]);
	    count++;
	}
	t = strtok(NULL, " ");
	wordfree(&p);
  }
  return result;
}*/

char** stringToArray(char* input){
  char buf[BUFFSIZE];
  strcpy(buf, input);
  char* t = strtok(buf, " ");
  int count = 1;
  while(strtok(NULL, " "))
    count++;
  char** argv = malloc((count+1)*sizeof(char *));
  argv[count] = NULL;
  strcpy(buf, input);
  t = strtok(buf, " ");
  count = 0;
  while(t){
    int len = strlen(t);
    argv[count] = malloc(sizeof(char)*(len+1));
    strcpy(argv[count], t);
    count++;
    t = strtok(NULL, " ");
  }
  return argv;
}

void destroyStrArray(char** argv){
  int count = 0;
  while(argv[count]){
    free(argv[count]);
    count++;
  }free(argv);
}

void readInput(char* buf){
	char buffer[BUFFSIZE];
	int len;
	int run = 1;

	while(run){
	    if(fgets(buffer, BUFFSIZE, stdin)!=NULL){
		len = (int)strlen(buffer);
		buffer[len-1] = '\0';
		strcpy(buf, buffer);
		run = 0;
	    }
	}
printf("%s\n", buf);
}
