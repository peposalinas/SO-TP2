#include "scheduler.h"

// En principio solo existe 1 prioridad (la 4)

/*Remueve un proceso de la lista de "READY"*/
static process_t *removeFromList(process_t *curr, uint32_t pid, int *gotRemoved);

/*Loop infinito que printea "Doing sth"*/
static void i_have_to_be_doin_sth();

static int firstTime = 1;

static process_t *ready_prio_4 = NULL;
static process_t *ready_prio_4_end = NULL;

/*
static process_t *ready_prio_1 = NULL;
static process_t *ready_prio_2 = NULL;
static process_t *ready_prio_3 = NULL;


static process_t *ready_prio_1_end = NULL;
static process_t *ready_prio_2_end = NULL;
static process_t *ready_prio_3_end = NULL;
*/

static process_t *blocked = NULL;

static void i_have_to_be_doin_sth()
{
    while (1)
        ncPrint("Doing sth");
}

void schedulerInit()
{
    firstTime--;

    // Creamos memoryManager del kernel
    void *dir1 = (void *)0x600000;
    void *dir2 = (void *)0x700000;
    createMemoryManager(dir1, dir2);

    // Creamos el 1er proceso
    char *argv2[] = {"i_have_to_be_doin_sth"};
    uint32_t firstPID = createProcess(i_have_to_be_doin_sth, 1, argv2);
    return ready_prio_4->stack;
}
void schedulerAddProcess(process_t *proc)
{
    if (ready_prio_4_end == NULL)
    {
        ready_prio_4_end = proc;
        ready_prio_4 = proc;
    }
    else
    {
        ready_prio_4_end->next = proc;
    }
}
uint32_t *schedulerRun(uint32_t *current_stack_pointer)
{
    // Correr el próximo programa
    // Si hay un proceso prioridad 4, lo corremos
    if (firstTime)
    {
        schedulerInit();
    }
    else
    {
        while (ready_prio_4)
        {
            // Si no es el que estábamos corriendo o no hay otro proceso prioridad 4, lo corremos
            if (ready_prio_4->stack_pointer != current_stack_pointer || ready_prio_4->next == NULL)
                return ready_prio_4->stack_pointer;
            // Si no, iteramos
            process_t *aux = ready_prio_4;
            ready_prio_4 = ready_prio_4->next;
            ready_prio_4_end = aux;
        }
        /*
        while (ready_prio_3)
        {
            // Si no es el que estábamos corriendo o no hay otro proceso prioridad 3, lo corremos
            if (ready_prio_3->stack_pointer != current_stack_pointer || ready_prio_3->next == NULL)
                return ready_prio_3->stack_pointer;
            // Si no, iteramos
            process_t *aux = ready_prio_3;
            ready_prio_3 = ready_prio_3->next;
            ready_prio_3_end = aux;
        }
        while (ready_prio_2)
        {
            // Si no es el que estábamos corriendo o no hay otro proceso prioridad 2, lo corremos
            if (ready_prio_2->stack_pointer != current_stack_pointer || ready_prio_2->next == NULL)
                return ready_prio_2->stack_pointer;
            // Si no, iteramos
            process_t *aux = ready_prio_2;
            ready_prio_2 = ready_prio_2->next;
            ready_prio_2_end = aux;
        }
        while (ready_prio_1)
        {
            // Si no es el que estábamos corriendo o no hay otro proceso prioridad 1, lo corremos
            if (ready_prio_1->stack_pointer != current_stack_pointer || ready_prio_1->next == NULL)
                return ready_prio_1->stack_pointer;
            // Si no, iteramos
            process_t *aux = ready_prio_1;
            ready_prio_1 = ready_prio_1->next;
            ready_prio_1_end = aux;
        }
        */
    }
}

int schedulerKillProcess(uint32_t pid)
{
    int gotRemoved = 0;
    ready_prio_4 = removeFromList(ready_prio_4, pid, &gotRemoved);

    //@TODO: crear un ADT de lista y agregar esto ahí
    // Si me quedé sin procesos (no debería pasar), asigno NULL al último también
    if (!ready_prio_4)
    {
        ready_prio_4_end = NULL;
    }

    /*
    if (!gotRemoved)
    {
        ready_prio_3 = removeFromList(ready_prio_3, pid, &gotRemoved);
        if (!ready_prio_3) {
        ready_prio_3_end = NULL;
    }
    }
    if (!gotRemoved)
    {
        ready_prio_2 = removeFromList(ready_prio_2, pid, &gotRemoved);
        if (!ready_prio_2) {
        ready_prio_2_end = NULL;
    }
    }
    if (!gotRemoved)
    {
        ready_prio_1 = removeFromList(ready_prio_1, pid, &gotRemoved);
        if (!ready_prio_1) {
        ready_prio_1_end = NULL;
    }
    }
    */
    return gotRemoved;
}

static process_t *removeFromList(process_t *curr, uint32_t pid, int *gotRemoved)
{
    if (curr == NULL)
    {
        return curr;
    }
    if (curr->pid == pid)
    {
        (*gotRemoved)++;
        return curr->next;
    }
    curr->next = removeFromList(curr->next, pid, gotRemoved);
    return curr;
}