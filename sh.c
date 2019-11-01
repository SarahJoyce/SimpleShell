#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <wordexp.h>
#include "sh.h"
#include "stringhelper.c"

int sh( int argc, char **argv, char **envp )
{
  char buffer[BUFFSIZE];
  char *prompt = calloc(PROMPTMAX, sizeof(char));
  char *commandline = calloc(MAX_CANON, sizeof(char));
  char *command, *arg, *commandpath, *p, *pwd, *owd;
  arg = calloc(MAX_CANON, sizeof(char));
  char **args; //calloc(MAXARGS, sizeof(char*));
  int uid, i, status, argsct, go = 1;
  struct passwd *password_entry;
  char *homedir, cwd[PATH_MAX];
  struct pathelement *pathlist;

  uid = getuid();
  password_entry = getpwuid(uid);               /* get passwd info */
  homedir = password_entry->pw_dir;		/* Home directory to start
						  out with*/
     
  if ( (pwd = getcwd(NULL, PATH_MAX+1)) == NULL )
  {
    perror("getcwd");
    exit(2);
  }
  owd = calloc(strlen(pwd) + 1, sizeof(char));
  memcpy(owd, pwd, strlen(pwd));
  strcpy(prompt, password_entry->pw_name);

  #if debug
  getcwd(cwd, sizeof(cwd));
  printf("CWD:\t%s\n", cwd);
  printf("Home:\t%s\n", homedir);
  printf("Prompt:%s\n", prompt);
  #endif

/* Put PATH into a linked list */
    pathlist = get_path();
  
  getcwd(cwd, sizeof(cwd));
  while (go){
    /* print your prompt */
    printf("%s:%s>", prompt, cwd);
    /* get command line and process */
    readInput(commandline);
    strcpy(arg, commandline);
    args = stringToArray(arg);
    /* check for each built in command and implement */
	if(args[0][0] == '\0'){
	  //handle
	}else if(strcmp(args[0], "token")==0){
	   wordexp_t p;
           char **w;
           int i;
	   if(args[1]!=NULL){
             wordexp(args[1], &p, 0);
	   }
           w = p.we_wordv;
           for (i = 0; i < p.we_wordc; i++)
               printf("%s\n", w[i]);
           wordfree(&p);
	}else if(strcmp(args[0], "exit")==0){ //exit command
	  go = 0;
	}else if(strcmp(args[0],"which")==0){
	  char* result;
	  result = which(args[1], pathlist);
	  if(result){
	   printf("%s\n", result);
	   free(result);
	  }
    }else if(strcmp(args[0],"where")==0){
	  where(args[1], pathlist);
    }else if(strcmp(args[0],"cd")==0){
	int success = 0;
	if(args[1]==NULL){
	  success = chdir(homedir);
	}else{
	  success = chdir(args[1]);
	}

	if(success == 0){
	    getcwd(cwd, sizeof(cwd));
	}
    }else if(strcmp(args[0],"pwd")==0){
	printf("%s\n", pwd);
    }else if(strcmp(args[0],"list")==0){
	list(args[1]);
    }else if(strcmp(args[0],"pid")==0){
	printf("%d\n", getpid());
    }else if(strcmp(args[0],"kill")==0){
	int signal = 15;
	pid_t pid = 0;
	if(args[1]!=NULL && args[2]!=NULL){
	  signal = atoi(args[1]);
	  pid = atoi(args[2]);
	  if(signal<0){
	    signal = -1*signal;
	  }
	  kill(pid, signal);
	}else if(args[1]!=NULL){
	  pid = atoi(args[1]);
	  kill(pid, signal);
	}
    }else if(strcmp(args[0],"prompt")==0){		
	if(args[1]==NULL){
	  char* input = malloc(sizeof(char)*BUFFSIZE);
	  strcpy(input, "\0");
	  printf("Please enter a prompt: ");
	  while(strlen(input)==0 || strcmp(input, "\n")==0){
		  readInput(input);
	  }
	  strcpy(prompt, input);
	  free(input);
	}else{
	  strcpy(prompt, args[1]);
	}
    }else if(strcmp(args[0],"printenv")==0){
	int i=0;
	if(args[1]==NULL){
	  while(envp[i]!=NULL){
	    printf("%s\n", envp[i]);
	    i++;
	  }
	}else{
	  while(envp[i]!=NULL){
	    char **env = &args[1];
	    printf("%s\n", getenv(*env));
	    i++;
	  }
	}
    /*}else if(strcmp(args[0],"setenv")==0){
	int i=0;
	if(args[1]==NULL){
	  while(envp[i]!=NULL){
	    printf("%s\n", envp[i]);
	    i++;
	  }
	}else if(args[1]!=NULL && args[2]==NULL){
	  setenv(args[1], "", 1);
	}else{
	  setenv(args[1], args[2], 1);
	}*/
    }else{
	fprintf(stderr, "%s: Command not found.\n", args[0]);
	/*  else  program to exec */
	/* find it */
	/*char *cmd = which(args[0], pathlist);
	pid_t pid = fork();*//* do fork(), execve() and waitpid() */

	/*if(pid>0){
		free(cmd);
		waitpid(pid, NULL, 0);
	}else{
	    if(execve(cmd,args,envp)<0){
		fprintf(stderr, "%s: Command not found.\n", args[0]);
		exit(0);
	}*/
    }
 
  destroyStrArray(args);
  }
  destroyPath(pathlist);
  free(arg);
  free(prompt);
  free(commandline);
  free(owd);
  free(pwd);
  return 0;
} /* sh() */

char *which(char *command, struct pathelement *pathlist){
   /* loop through pathlist until finding command and return it.  Return
   NULL when not found. */
   char* path = malloc(sizeof(char)*PATH_MAX);
   if(command[0] == '/' || command[0] == '.'){
	     sprintf(path, "%s", command);
	     if (access(path, X_OK) == 0) {
	       printf("[%s]\n", path);
	       return path;
	     }
  }else{
	while (pathlist) {         // WHICH
	     sprintf(path, "%s/%s", pathlist->element, command);
	     if (access(path, X_OK) == 0) {
	       printf("[%s]\n", path);
	       return path;
	     }
	    pathlist = pathlist->next;
	   }   
  }
 free(path);
 return NULL;
} /* which() */

char *where(char *command, struct pathelement *pathlist){
  /* similarly loop through finding all locations of command */
char* path = malloc(sizeof(char)*PATH_MAX);
if(command[0] == '/' || command[0] == '.'){
	     sprintf(path, "%s", command);
	     if (access(path, F_OK) == 0) {
	       printf("[%s]\n", path);
	       return path;
	     }
  }else{
  while (pathlist) {         // WHERE
    sprintf(path, "%s/%s", pathlist->element, command);
    if (access(path, F_OK) == 0)
      printf("[%s]\n", path);
      pathlist = pathlist->next;
    }free(path);
}
} /* where() */

void list (char *dir){
  /* see man page for opendir() and readdir() and print out filenames for
  the directory passed */
  char* path = malloc(sizeof(char)*(PATH_MAX+1));
  struct dirent *d;

  if(dir==NULL){
    strcpy(path, ".");
  }else{
    strcpy(path, dir);
  }

  DIR *directory = opendir(path);
  if(directory==NULL){
   printf("NULL");
  }else{
    d = readdir(directory);
    while(d!=NULL){
	printf("%s\n", d->d_name);
	d=readdir(directory);
    }free(d);
  }free(directory);
   free(path);
} /* list() */
