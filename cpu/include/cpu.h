#ifndef CPU_H
#define CPU_H
#include<stdlib.h>
#include <stdio.h>
#include <commons/log.h>
#include <stdbool.h>
#include "client.h"
#include "server.h"
#include "tests.h"

t_config* config;
int conexion;
int conexionMemoria;

int contadorIdPCB;
typedef struct {
	int entradasTLB;
	char* reemplazoTLB;
	int retardoInstruccion;
	char* ipMemoria;
	char* puertoMemoria;
	char* puertoEscuchaDispatch;
	char* puertoEscuchaInterrupt;

	char* ipCPU;

}t_configCPU;

t_configCPU configCPU;

t_configKernel configKernel;

bool interrupciones;

int socketAceptadoDispatch;

t_configCPU extraerDatosConfig(t_config* );

void iniciar_servidor_dispatch();
void iniciar_servidor_interrupt();
void conectar_memoria();
char* registroToString(t_registro );
char* instruccionToString(t_instCode );
uint32_t matchearRegistro(t_registros ,t_registro );



// LISTAS
t_list *LISTA_NEW;
t_list *LISTA_READY;
t_list *LISTA_EXEC;
t_list *LISTA_BLOCKED;
t_list *LISTA_EXIT;
t_list *LISTA_SOCKETS;

// MUTEX
pthread_mutex_t mutex_creacion_ID;
pthread_mutex_t mutex_lista_new;
pthread_mutex_t mutex_lista_ready;
pthread_mutex_t mutex_lista_exec;
pthread_mutex_t mutex_lista_blocked;
pthread_mutex_t mutex_lista_exit;

// SEMAFOROS
sem_t sem_planif_largo_plazo;
sem_t contador_multiprogramacion;
sem_t contador_pcb_running;
sem_t sem_ready;
sem_t sem_bloqueo;
sem_t sem_procesador;


sem_t sem_agregar_pcb;
sem_t sem_hay_pcb_lista_new;
sem_t sem_hay_pcb_lista_ready;
sem_t sem_pasar_pcb_running;

#endif