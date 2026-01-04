// o programa prepare é o responsável por criar os ficheiros binários das tarefas

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>  //open
#include <string.h> //sprintf
#include "task.h"



int main(int argc, char *argv[]) {
    // VALIDAÇÃO DOS ARGUMENTOS PASSADOS PELO UTILIZADOR
    if (argc != 3) {
        printf("O programa deve ser executado desta forma: ./prepare <task_id> <duration_in_seconds>\n");
        return 1;
    }

    // CONVERSÃO DOS ARGUMENTOS PASSADOS PELO UTILIZADOR NO TERMINAL PARA int
    int task_id = atoi(argv[1]);
    int duration = atoi(argv[2]);

    // VALIDAÇÃO DO task_id E duration COMO VALORES POSITIVOS
    if (task_id <= 0 || duration <= 0) {
        printf("Erro: a identificação da tarefa e a duração devem ser valores positivos.\n");
        return 1;
    }

    /* CRIAÇÃO DA STRUCT Task E
    PREENCHIMENTO DOS SEUS DADOS COM OS VALORES PASSADOS PELO UTILIZADOR */
    Task t;
    t.id = task_id;
    t.duration = duration;

    // CRIAÇÃO DO NOME DO FICHEIRO BINÁRIO DE CADA TAREFA task
    char buf[1024];
    sprintf(buf, "tasks/task_%d.bin", t.id);

    // ABERTURA/CRIAÇÃO DO FICHEIRO E VERIFICAÇÃO
    int fd = open(buf, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        printf("Erro ao abrir o ficheiro %s.\n", buf);
        return 1;
    }

    // ESCRITA DA ESTRUTURA DA task NO FICHEIRO BINÁRIO E FECHO DO FICHEIRO
    write(fd, &t, sizeof(Task));
    close(fd);

    // IMPRESSÃO DE CONFIRMAÇÃO
    printf("Tarefa %d guardada com duração %d segundos.\n", t.id, t.duration);
    return 0;
    
}