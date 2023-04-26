#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#define SIGHUP  1   /* Hangup the process */
#define SIGINT  2   /* Interrupt the process */
#define SIGQUIT 3   /* Quit the process */
#define SIGILL  4   /* Illegal instruction. */
#define SIGTRAP 5   /* Trace trap. */
#define SIGABRT 6   /* Abort. */

int main(int argc, char* argv[]) {
    if (argc != 3) { //Caso o numero de argumentos seja insuficiente
        printf("Comando: %s <PID> <signal>\n", argv[0]);
        return 1;
    }

    pid_t pid = atoi(argv[1]); //Conversao dos argumentos para inteiros
    int signal = atoi(argv[2]);

    int result = kill(pid, signal); //kill() retorna -1 se o processo nao existe
    //Se pid = 0, kill manda o sinal signal a todos os processos cujos process group ID sao
    //iguais ao do processo de origem

    if (result == 0) {
        printf("O sinal de numero %d foi enviado ao processo de PID %d\n", signal, pid);
    } else {
        int error_number = errno;

        switch(error_number) {
            case 22:
                printf("ERRO. Nao foi possivel enviar o sinal %d ao processo de PID %d. Sinal invalido.\n", signal, pid);
                break;
            case 1:
                printf("ERRO. Nao foi possivel enviar o sinal %d ao processo de PID %d. Permissao negada.\n", signal, pid);
                break;
            case 3:
                printf("ERRO. Nao foi possivel enviar o sinal %d ao processo de PID %d. Processo nao existe.\n", signal, pid);
                break;
            default:
                printf("ERRO. Nao foi possivel enviar o sinal %d ao processo de PID %d. Erro numero %d.\n", signal, pid, error_number);
        }
    }
    return 0;
}