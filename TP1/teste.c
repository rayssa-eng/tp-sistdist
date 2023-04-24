#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#define ERRO               1

void signal_handler(int signal) {

    switch (signal) {
        case SIGUSR1:
            printf("Sinal SIGUSR1 recebido\n");
            break;
        case SIGUSR2:
            printf("Sinal SIGUSR2 recebido\n");
            break;
        case SIGQUIT:
            printf("Sinal SIGQUIT recebido, logo o processo irá terminar\n");
            exit(0);
            break;
    }
}


int enviaSinal(pid_t pidDestino, int signal){

    printf("Enviando sinal\n");

    if (getpgid(pidDestino) >= 0) {
        printf("Processo existe\n");
        kill(pidDestino, signal);
        printf("Sinal enviado\n");
        sleep(2);
        return 0;
    
    }else{
        printf("Processo não existe\n");

        return 1;
    }
}

int recebeSinal(int tipoWait){

    printf("Recebendo sinal\n");

    pid_t pid;
    pid = getpid(); //getpid retorna pid do processo chamado
    printf("Pid é %d \n",pid); //exibindo pid

    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);
    signal(SIGQUIT, signal_handler);

    if (tipoWait == 0) {
        while (1); // busy wait
    }else {
        while(1){ 
            pause(); 
        } // blocking wait
    }

    return 0;
}

int main(){
    
    int opcao;

    printf("Esse programa recebe e envia sinais!\n");

    printf("Você gostaria de receber ou enviar sinais?\n");

    printf("1 - Receber\n2 - Enviar\n");

    scanf("%d", &opcao);
    switch (opcao)
    {
    case 1:;

        int tipoWait;

        printf("Digite 0 para busy wait ou 1 para blocking wait:\n");

        scanf("%d", &tipoWait);

        recebeSinal(tipoWait);

        break;

    case 2:;
     
        int pidDestino;
        int sinal;

        printf("Qual o pid de Destino?\n");
        scanf("%d", &pidDestino);

        printf("Digite o número do  sinal desejado:\n");
        printf("10 - SIGUSR1; 12 - SIGUSR2; 3 - SIGQUIT (Finaliza o programa)\n");
        scanf("%d", &sinal);

        
        enviaSinal(pidDestino,sinal);
        break;    
    default:

        break;
    }

    ///Decidir se recebe sinal ou envia 
    
    /*pid_t pidDestino;
    pidDestino = 101513;
    
    enviaSinal(pidDestino, 15);*/
    
    return 0;

}