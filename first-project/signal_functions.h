void sig_chld_handler(int signo);
void install_sigchld_handler();

void sig_usr1_handler(int signo);
void install_sigusr1_handler();

void sig_super_usr1_handler(int signo);
void install_super_sigusr1_handler();
