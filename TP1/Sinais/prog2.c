#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

volatile sig_atomic_t flag = 0;

void signal_handler(int sig) {
    switch(sig) {
        case SIGINT:
            printf("Sinal SIGINT recebido.\n");
            break;
        case SIGTERM:
            printf("Sinal SIGTERM recebido.\n");
            break;
        case SIGQUIT:
            printf("Sinal SIGQUIT recebido.\n");
            break;
        case SIGTSTP:
            printf("Sinal SIGTSTP recebido. Parando processo.\n");
            flag = 1;
            break;
        default:
            printf("Recebemos um sinal ai. Se esse sinal parar o processo, parou.\n");
            flag = 1;
    }
//    flag = 1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) { //Caso o numero de argumentos seja insuficiente
        printf("Comando: %s <busy_type>, 0 para busy wait e 1 para blocking wait\n", argv[0]);
        return 1;
    }

    int busy_type = atoi(argv[1]);

    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = &signal_handler; //aponta pra uma funcao signal_handler que ira ditar como o programa lida com um certo sinal
    sa.sa_flags = 0; //sigtstp e scanf
    sigaction(SIGTSTP, &sa, NULL); //funcao utilizada para signal handling. o ultimo parametro poderia guardar o handle
    //anterior, mas nao fazemos questao disso
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    if (busy_type == 0) {
        // Busy waiting o sinal
        while (!flag) {
            printf("Estamos esperando um sinal e estamos ocupadas.\n");
            sleep(2);
        }
    } else {
        printf("Estamos esperando um sinal...\n");

        //Blocking ate um sinal ser recebido
        sigsuspend(&sa.sa_mask);

        printf("Sinal recebido. Continuando...\n");

        int x;

        printf("Digite um numero de 0 a 9: \n");
        scanf("%d", &x);
        printf("Resultado %d * 10 = %d\n", x, x * 10);

        //Continua a execucao
    }
    return 0;
}

/*SIGTERM (15): This is the default signal sent by the kill command and is used to request a process to terminate gracefully.

SIGKILL (9): This is a forceful signal that is used to immediately terminate a process. Unlike SIGTERM, this signal cannot be caught or ignored by the process.

SIGINT (2): This signal is sent to a process when the user presses the interrupt key (usually Ctrl+C) and is often used to request a process to stop executing.

SIGQUIT (3): This signal is similar to SIGINT, but is sent when the user presses the quit key (usually Ctrl+) and can be used to request a process to dump core and terminate.

SIGSTOP (19) and SIGTSTP (20): These signals are used to suspend a process and can be sent by the user or operating system.

SIGHUP (1): This signal is sent when a process's controlling terminal is closed or when the process receives a hangup signal from the operating system.

SIGUSR1 (10) and SIGUSR2 (12): These signals are user-defined and can be used for any purpose by the process or the system administrator.*/