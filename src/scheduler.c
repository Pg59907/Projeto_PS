/* o programa scheduler é o responsável por ler os ficheiros criados pelo prepare;
 por carregar as tarefas todas para memória e executar as tarefas pela política FCFS */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep
#include <fcntl.h> // open
#include <string.h> // sprintf / snprintf
#include <time.h>
#include <sys/wait.h> // wait
#include "task.h"

int compare_tasks(const void *a, const void *b) {
    Task *t1 = (Task *)a;
    Task *t2 = (Task *)b;
    return t1->duration - t2->duration;
}

int main(int argc, char *argv[]) {
    // VALIDAÇÃO DOS ARGUMENTOS PASSADOS PELO UTILIZADOR
    if (argc < 2 || argc > 4) {
        printf("Executar o programa sem otimizações (tarefas sequenciais + FCFS): ./scheduler <num_tasks>\n"
            "Executar o programa com otimização 1 (tarefas em paralelo + FCFS): ./scheduler <num_tasks> <num_parallel>\n"
            "Executar o programa com otimização 2 (tarefas sequenciais + SJF): ./scheduler <num_tasks> sjf\n"
            "Executar o programa com as 2 otimizações (tarefas em paralelo + SJF): ./scheduler <num_tasks> <num_parallel> sjf\n");
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

    int num_parallel = 0;
    int use_sjf = 0;        // FCFS default

    for (int i = 2; i < argc; i++) {

        if (strcmp(argv[i], "sjf") == 0) {
            use_sjf = 1;
        }
        else {
            int value = atoi(argv[i]);

            if (value <= 0) {
                printf("Erro: argumento inválido '%s'\n", argv[i]);
                return 1;
            }

            if (num_parallel != 0) {
                printf("Erro: num_parallel especificado mais de uma vez.\n");
                return 1;
            }

            num_parallel = value;
        }
    }

    if (num_parallel == 0) {
        num_parallel = 1;
    }

    // CICLO PARA LEITURA DOS FICHEIROS BINÁRIOS ESCRITOS PELO PROGRAMA prepare
    for (int i = 0; i < num_tasks; i++) {
        char buf[1024];
        snprintf(buf, sizeof(buf), "tasks/task_%d.bin", i + 1);

        int fd = open(buf, O_RDONLY);

        if (fd < 0) {
            perror("Erro ao abrir o ficheiro.");
            free(tarefas);
            return 1;
        }

        ssize_t bytes_read = read(fd, &tarefas[i], sizeof(Task));
        close(fd);
        
        if (bytes_read != sizeof(Task)) {
            printf("Erro ao ler a tarefa %d\n", i + 1);
            free(tarefas);
            return 1;
        }
    }

    //SJF
    if (use_sjf) {
        qsort(tarefas, num_tasks, sizeof(Task), compare_tasks);
        printf("Escalonamento SJF selecionado.\n");
    } else {
        printf("Escalonamento FCFS selecionado.\n");
    }

    // EXECUÇÃO DAS TAREFAS EM PARALELO
    time_t inicio = time(NULL);

    int running = 0;
    int next_task = 0;

    while (next_task < num_tasks || running > 0) {

        while (running < num_parallel && next_task < num_tasks) {
            pid_t pid = fork();
            if (pid == 0) {
                Task t = tarefas[next_task];
                printf("A executar tarefa %d com duração %d segundos.\n", t.id, t.duration);
                sleep(t.duration);
                printf("Tarefa %d concluída.\n", t.id);
                exit(0);
            }

            running++;
            next_task++;
        }

        wait(NULL);
        running--;
    }

    time_t fim = time(NULL);

    // CÁLCULO DAS ESTATÍSTICAS GLOBAIS
    int turnaround = (int)(fim - inicio);
    float turnaround_medio = (float) turnaround/num_tasks;

    // IDENTIFICAÇÃO DO MODO DE EXECUÇÃO
    char *modo_execucao;

    if (argc == 2) {
        modo_execucao = "Sem otimizações (FCFS sequencial)";
    }
    else if (argc == 3 && strcmp(argv[2], "sjf") != 0) {
        modo_execucao = "Otimização 1 (FCFS paralelo)";
    }
    else if (argc == 3 && strcmp(argv[2], "sjf") == 0) {
        modo_execucao = "Otimização 2 (SJF sequencial)";
    }
    else if (argc == 4 && strcmp(argv[3], "sjf") == 0) {
        modo_execucao = "Otimizações 1 e 2 (SJF paralelo)";
    }

    // CRIAÇÃO DO FICHEIRO DE ESTATÍSTICAS GLOBAIS
    int fd = open("estatisticas/estatisticas_globais.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("Erro ao criar o ficheiro 'estatisticas_globais'.\n");
        free(tarefas);
        return 1;
    }

    // ESCRITA DAS ESTATÍSTICAS GLOBAIS NO FICHEIRO
    char buf[1024];
    int len = sprintf(buf, "\n--- %s ---\nTotal Tarefas Executadas: %d\nTurnaround Time Médio: %.2f segundos\n", modo_execucao, num_tasks, turnaround_medio);
    
    ssize_t res = write(fd, buf, len);
    if (res != len) {
        perror("Erro ao escrever no ficheiro 'estatisticas_globais'.");
        close(fd);
        free(tarefas);
        return 1;
    }

    printf("\n");
    printf("Ficheiro 'estatisticas_globais' atualizado com sucesso.\n");

    close(fd);
    free(tarefas);
    return 0;
}