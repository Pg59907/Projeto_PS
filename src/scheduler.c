/* o programa scheduler é o responsável por ler os ficheiros criados pelo prepare;
 por carregar as tarefas todas para memória e executar as tarefas pela política FCFS */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep
#include <fcntl.h> // open
#include <string.h> // sprintf / snprintf
#include <time.h>
#include "task.h"


int main(int argc, char *argv[]) {
    // VALIDAÇÃO DOS ARGUMENTOS PASSADOS PELO UTILIZADOR
    if (argc != 2) {
        printf("O programa deve ser executado desta forma: ./scheduler <num_tasks>\n");
        return 1;
    }

    // CONVERSÃO DO ARGUMENTO PASSADO PELO UTILIZADOR NO TERMINAL PARA int
    int num_tasks = atoi(argv[1]);

    // VALIDAÇÃO DO num_tasks COMO VALORES POSITIVOS
    if (num_tasks <= 0) {
        printf("Erro: num_tasks deve ser positivo.\n");
        return 1;
    }

    // ALOCAÇÃO DE MEMÓRIA PARA AS TAREFAS E VALIDAÇÃO
    Task *tarefas = malloc(num_tasks * sizeof(Task));
    if (tarefas == NULL) {
        perror("Erro ao alocar memória para as tarefas.");
        return 1;
    }

    // CICLO PARA LEITURA DOS FICHEIROS BINÁRIOS ESCRITOS PELO PROGRAMA prepare
    for (int i = 0; i < num_tasks; i++) {
        //
        char buf[1024];
        snprintf(buf, sizeof(buf), "tasks/task_%d.bin", i + 1);

        // ABERTURA DOS FICHEIROS BINÁRIOS (PARA LEITURA APENAS) E VERIFICAÇÃO
        int fd = open(buf, O_RDONLY);

        if (fd < 0) {
            perror("Erro ao abrir o ficheiro.");
            free(tarefas);  //quando dá erro permite que a memória já alocada seja libertada
            return 1;
        }

        //LEITURA DOS FICHEIROS BINÁRIOS
        ssize_t bytes_read = read(fd, &tarefas[i], sizeof(Task));
        close(fd);
        
        if (bytes_read != sizeof(Task)) {
            printf("Erro ao ler a tarefa %d\n", i + 1);
            free(tarefas);
            return 1;
        }
    }


    // INÍCIO DO TEMPO DE EXECUÇÃO DAS TAREFAS
    time_t inicio = time(NULL);

    // EXECUÇÃO DAS TAREFAS (FCFS)
    for (int i = 0; i < num_tasks; i++) {
        Task t = tarefas[i];
        printf("A executar tarefa %d com duração %d segundos.\n", t.id, t.duration);
        sleep(t.duration); // pausa a execução do programa scheduler durante o tempo da tarefa
        printf("Tarefa %d concluída.\n", t.id);
    }

    // FIM DO TEMPO DE EXECUÇÃO DAS TAREFAS
    time_t fim = time(NULL);


    // CÁLCULO DAS ESTATÍSTICAS GLOBAIS
    int turnaround = (int)(fim - inicio);
    float turnaround_medio = (float) turnaround/num_tasks;

    // CRIAÇÃO DO FICHEIRO DE ESTATÍSTICAS GLOBAIS
    int fd = open("estatisticas/estatisticas_globais.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Erro ao criar o ficheiro 'estatisticas_globais'.\n");
        free(tarefas);
        return 1;
    }

    // ESCRITA DAS ESTATÍSTICAS GLOBAIS NO FICHEIRO
    char buf[1024];
    int len = sprintf(buf, "ESTATÍSTICAS GLOBAIS\n\nTotal Tarefas Executadas: %d\nTurnaround Time Médio: %.0f segundos\n", num_tasks, turnaround_medio);
    
    ssize_t res = write(fd, buf, len);
    if (res != len) {
        perror("Erro ao escrever no ficheiro 'estatisticas_globais'.");
        close(fd);
        free(tarefas);
        return 1;
    }
    printf("\n");
    printf("Ficheiro 'estatisticas_globais' criado com sucesso.\n");

    // FECHO DO FICHEIRO E LIBERTAÇÃO DA MEMÓRIA ALOCADA PARA AS TAREFAS
    close(fd);
    free(tarefas);
    return 0;
    
}