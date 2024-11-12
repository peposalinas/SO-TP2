// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "libc.h"

#define INITIAL_PHILOSOPHERS 5
#define MIN_PHILOSOPHERS 3
#define MAX_PHILOSOPHERS 8
#define MUTEX_SEM 30
#define EATING_SECONDS 1
#define THINKING_SECONDS 1
#define MAX_NUMBER_CHARS 3
#define QUIT_LETTER 'q'
#define ADD_LETTER 'a'
#define REMOVE_LETTER 'r'

#define LEFT(i) (((i) + philosophersAmount - 1) % philosophersAmount)
#define RIGHT(i) (((i) + 1) % philosophersAmount)
#define PHILOSOPHER_SEM(i) i

typedef enum
{
    THINKING,
    HUNGRY,
    EATING
} philosopher_state;

typedef struct
{
    int pid;
    philosopher_state state;
} philosopher_t;

static int philosophersAmount = 0;
philosopher_t philosopherArray[MAX_PHILOSOPHERS];

static int philosopher(int argc, char **argv);
static void addPhilosopher(int index);
static void removePhilosopher(int index);
static void takeForks(int i);
static void putDownForks(int i);
static void test(int i);
static void displayTableState();

char *argvs2[MAX_PHILOSOPHERS][1] = {{"0"}, {"1"}, {"2"}, {"3"}, {"4"}, {"5"}, {"6"}, {"7"}};

int philosophersRun(int argc, char **argv)
{
    printf("Bienvenido a la cena de los filosofos!\nPresione 'q' para salir, 'a' para agregar un filosofo y 'r' para remover uno\n");
    philosophersAmount = 0;
    if (openSem(MUTEX_SEM, 1) == -1)
    {
        exitProc(-1);
    }

    for (int i = 0; i < MAX_PHILOSOPHERS; i++)
    {
        philosopherArray[i].pid = -1;
        philosopherArray[i].state = THINKING;
    }

    for (int i = 0; i < INITIAL_PHILOSOPHERS; i++)
    {
        addPhilosopher(i);
    }

    char command = '\0';
    while ((command = getChar()) != QUIT_LETTER)
    {
        switch (command)
        {
        case ADD_LETTER:
            addPhilosopher(philosophersAmount);
            break;
        case REMOVE_LETTER:
            removePhilosopher(philosophersAmount - 1);
            break;
        }
    }
    waitSemMutex(MUTEX_SEM);
    for (int i = 0; i < philosophersAmount; i++)
    {
        killProcess(philosopherArray[i].pid);
        closeSem(PHILOSOPHER_SEM(i));
    }
    postSemMutex(MUTEX_SEM);
    closeSem(MUTEX_SEM);
    exitProc(0);
    return 0;
}

static void displayTableState()
{
    for (int i = 0; i < philosophersAmount; i++)
    {
        printf("%s ", philosopherArray[i].state == EATING ? "E" : ".");
    }
    printf("\n");
}

static void addPhilosopher(int index)
{
    waitSem(MUTEX_SEM);
    if (philosophersAmount == MAX_PHILOSOPHERS)
    {
        printf("No se pueden agregar mas filosofos\n");
        postSemMutex(MUTEX_SEM);
        return;
    }
    if (openSem(PHILOSOPHER_SEM(index), 0) == -1)
    {
        printf("Error al abrir semaforo\n");
        postSem(MUTEX_SEM);
        return;
    }

    philosopherArray[index].pid = createStandardProc("philosopher", philosopher, 2, argvs2[index]);
    if (philosopherArray[index].pid != -1)
    {
        philosophersAmount++;
    }
    displayTableState();
    postSem(MUTEX_SEM);
}

static void removePhilosopher(int index)
{

    waitSem(MUTEX_SEM);

    if (philosophersAmount == MIN_PHILOSOPHERS)
    {
        printf("No se pueden remover mas filosofos\n");
        postSemMutex(MUTEX_SEM);
        return;
    }

    printf("El filosofo %d ha comido demasiado y se retira\n", philosopherArray[index].pid);
    while (philosopherArray[LEFT(index)].state == EATING && philosopherArray[RIGHT(index)].state == EATING)
    {
        postSem(MUTEX_SEM);
        waitSem(PHILOSOPHER_SEM(index));
        waitSem(MUTEX_SEM);
    }

    killProcess(philosopherArray[index].pid);
    closeSem(PHILOSOPHER_SEM(index));

    philosopherArray[index].pid = -1;
    philosopherArray[index].state = THINKING;
    philosophersAmount--;
    postSem(MUTEX_SEM);
    displayTableState();
}

static int philosopher(int argc, char **argv)
{
    int i = atoi(argv[0]);

    printf("Soy el filosofo %d y estoy listo para comer una rica pasta!\n", philosopherArray[i].pid);
    philosopherArray[i].state = THINKING;
    while (1)
    {
        wait(THINKING_SECONDS);
        takeForks(i);
        wait(EATING_SECONDS);
        putDownForks(i);
    }
    exitProc(0);
    return 0;
}

static void takeForks(int i)
{
    waitSem(MUTEX_SEM);
    philosopherArray[i].state = HUNGRY;
    test(i);
    postSem(MUTEX_SEM);
    waitSem(PHILOSOPHER_SEM(i));
}

static void putDownForks(int i)
{
    waitSem(MUTEX_SEM);
    philosopherArray[i].state = THINKING;
    displayTableState();
    test(LEFT(i));
    test(RIGHT(i));
    postSem(MUTEX_SEM);
}

static void test(int i)
{
    if (philosopherArray[i].state == HUNGRY && philosopherArray[LEFT(i)].state != EATING &&
        philosopherArray[RIGHT(i)].state != EATING)
    {
        philosopherArray[i].state = EATING;
        displayTableState();
        postSem(PHILOSOPHER_SEM(i));
    }
}