#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define LOCK_FILE "/tmp/sem_lock_file"

int main() {
    int n = 10; // Número de terminais a serem abertos
    for (int i = 0; i < n; i++) {
        // Verificar se o arquivo de bloqueio existe
        int lock_file = open(LOCK_FILE, O_CREAT | O_EXCL, 0);
        if (lock_file == -1) {
            printf("O código já está em execução.\n");
            return 0;
        }

        // Redirecionar a saída padrão do programa para um arquivo
        char output_file[20];
        sprintf(output_file, "output%d.txt", i+1);
        int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
        dup2(output_fd, STDOUT_FILENO);

        // Executar o código
        system("./sem");

        // Fechar o arquivo de saída
        close(output_fd);

        // Abrir um novo terminal com o conteúdo do arquivo de saída
        char command[100];
        sprintf(command, "gnome-terminal --command='cat %s; /bin/bash'", output_file);
        system(command);

        // Remover o arquivo de bloqueio
        unlink(LOCK_FILE);
    }
    return 0;
}
