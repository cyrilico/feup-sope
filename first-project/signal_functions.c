#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "macros.h"
#include "signal_functions.h"

/* Don't really want to do anything with SIGCHLD signal */
void sig_chld_handler(int signo){
  int termination_status;
  waitpid(-1, &termination_status, WNOHANG);
  printf("A CHILD FROM PROCESS %d JUST TERMINATED WITH EXIT CODE %d\n", getpid(), WEXITSTATUS(termination_status));
}

void install_sigchld_handler(){
  struct sigaction act;

  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_handler = sig_chld_handler;

  if(!IS_OK(sigaction(SIGCHLD, &act, NULL))){
    perror("SIGCHLD Handler");
    exit(-1);
  }
}

/* Once received it will 'commit suicide' and send the same signal to its parent (recursive behavior) until it reaches the 'main' parent process */
void sig_usr1_handler(int signo){
  kill(getppid(), SIGUSR1);
  exit(0);
}

void install_sigusr1_handler(){
  struct sigaction act;

  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  act.sa_handler = sig_usr1_handler;

  if(!IS_OK(sigaction(SIGUSR1, &act, NULL))){
    perror("SIGUSR1 Handler");
    exit(-1);
  }
}

/* To be implemented only in the 'main' parent process: if he receives this signal, it only terminates, as we're not interested in killing it's parent process */ 
