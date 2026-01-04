#!/bin/bash

NUM_TASKS=5
PARALLEL=3

AMARELO="\e[33m"
RESET="\e[0m"

echo -e "${AMARELO}Modo sem otimizações (FCFS sequencial)${RESET}"
/usr/bin/time -f "Tempo real de execução: %e segundos\n\n" ./scheduler $NUM_TASKS

echo -e "${AMARELO}Modo com otimização 1 (FCFS paralelo)${RESET}"
/usr/bin/time -f "Tempo real de execução: %e segundos\n\n" ./scheduler $NUM_TASKS $PARALLEL

echo -e "${AMARELO}Modo com otimização 2 (SJF sequencial)${RESET}"
/usr/bin/time -f "Tempo real de execução: %e segundos\n\n" ./scheduler $NUM_TASKS sjf

echo -e "${AMARELO}Modo com otimizações 1 e 2 (SJF paralelo)${RESET}"
/usr/bin/time -f "Tempo real de execução: %e segundos\n\n" ./scheduler $NUM_TASKS $PARALLEL sjf