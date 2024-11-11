#include "libc.h"
#include "list.h"

#define INITIAL_PHILOSOPHERS 5
#define MIN_PHILOSOPHERS 3
#define MAX_PHILOSOPHERS 8
#define THINKING_TICKS 1
#define EATING_TICKS 1
#define MAX_NUMBER_CHARS 3
#define MUTEX_SEM 30 // Asegurarse que sea > a MAX_PHILOSOPHERS
#define QUIT_LETTER 'q'
#define ADD_LETTER 'a'
#define REMOVE_LETTER 'r'

typedef enum
{
    THINKING,
    EATING,
    HUNGRY
} philosopherState;

typedef struct
{
    int id;
    int pid;
    int hasLeftFork;
    int hasRightFork;
    philosopherState state;
} philosopher_t;

static int philosopher(int argc, char *argv[]);
static void takeForks(int philosopher);
static void takeFork(int philosopher, int fork);
static void putDownForks(int philosopher);
static void displayTableState();
static void addPhilo();
static void removePhilo();
static void think();
static void eat();
static int leftFork(int philosopher);
static int rightFork(int philosopher);

int currentPhilosopherAmount = INITIAL_PHILOSOPHERS;
LinkedList *philosWaitingForFork[MAX_PHILOSOPHERS];
philosopherState philosopherStates[MAX_PHILOSOPHERS];
int philosopherPids[MAX_PHILOSOPHERS];
philosopher_t philosophers[MAX_PHILOSOPHERS];
//@TODO: cambiar
char *argvs[MAX_PHILOSOPHERS][1] = {{"0"}, {"1"}, {"2"}, {"3"}, {"4"}, {"5"}, {"6"}, {"7"}};

int philosophersRun(int argc, char *argv[])
{
    char c;
    openSem(MUTEX_SEM, 1);
    printf("Bienvenido a la cena de los filosofos!\nPresione 'q' para salir, 'a' para agregar un filosofo y 'r' para remover uno\n");
    // char numberStr[MAX_NUMBER_CHARS];
    for (int i = 0; i < INITIAL_PHILOSOPHERS; i++)
    {
        philosWaitingForFork[i] = createLinkedList();
        philosophers[i].state = THINKING;
        if (openSem(i, 1) == -1)
        {
            printf("Error al abrir semaforo\n");
            exitProc(-1);
        }
        // itos(i, numberStr);
        // char *argvPhilo[] = {numberStr};
        philosophers[i].pid = createStandardProc("philosopher", philosopher, 1, argvs[i]);
    }
    displayTableState();
    while ((c = getChar()) != QUIT_LETTER)
    {
        switch (c)
        {
        case 'a':
            addPhilo();
            break;
        case 'r':
            removePhilo();
            break;
        }
    }
    waitSem(MUTEX_SEM);
    for (int i = 0; i < currentPhilosopherAmount; i++)
    {
        killProcess(philosophers[i].pid);
        closeSem(i);
        destroyLinkedList(philosWaitingForFork[i]);
    }
    postSem(MUTEX_SEM);
    closeSem(MUTEX_SEM);
    exitProc(0);
    return 0;
}

static void addPhilo()
{
    waitSem(MUTEX_SEM);
    // char numberStr[MAX_NUMBER_CHARS];
    if (currentPhilosopherAmount == MAX_PHILOSOPHERS)
    {
        printf("No se pueden agregar más filosofos\n");
        postSem(MUTEX_SEM);
        return;
    }
    philosWaitingForFork[currentPhilosopherAmount] = createLinkedList();
    philosophers[currentPhilosopherAmount].state = THINKING;
    if (openSem(currentPhilosopherAmount, 1) == -1)
    {
        printf("Error al abrir semaforo\n");
        exitProc(-1);
    }
    // itos(philo, numberStr);
    // char *argvPhilo[] = {numberStr};
    philosophers[currentPhilosopherAmount].pid = createStandardProc("philosopher", philosopher, 1, argvs[currentPhilosopherAmount]);

    currentPhilosopherAmount++;
    postSem(MUTEX_SEM);
}

static void removePhilo()
{
    waitSem(MUTEX_SEM);
    if (currentPhilosopherAmount == 0)
    {
        printf("No hay filósofos para remover\n");
        postSem(MUTEX_SEM);
        return;
    }
    else if (currentPhilosopherAmount == MIN_PHILOSOPHERS)
    {
        printf("No se pueden remover más filosofos\n");
        postSem(MUTEX_SEM);
        return;
    }
    int toRemove = currentPhilosopherAmount - 1;
    killProcess(philosophers[toRemove].pid);
    closeSem(toRemove);
    destroyLinkedList(philosWaitingForFork[toRemove]);
    for (int i = 0; i < toRemove; i++)
    {
        removeElem(philosWaitingForFork[i], toRemove);
    }
    // Solo tomo en cuenta el tenedor izquierdo, pues el derecho es el que remuevo
    if (philosophers[toRemove].hasLeftFork)
    {
    }
    currentPhilosopherAmount--;
    postSem(MUTEX_SEM);
}

static int philosopher(int argc, char *argv[])
{
    // Life is very busy as a philosopher

    if (argc != 1)
    {
        printf("Filosofo %d: Necesito conocer mi número!\n", getPID());
        exitProc(-1);
    }

    int philosopher = atoi(argv[0]);
    printf("Soy el filosofo %d y estoy listo para comer una rica pasta!\n", philosopher);

    while (1)
    {
        think();
        takeForks(philosopher);
        eat();
        putDownForks(philosopher);
    }
    exitProc(1);
    return 1;
}

static void takeForks(int philosopher)
{
    // A la derecha del filósofo i, está el tenedor i. A su izquierda, el tenedor i+1%currentPhilosopherAmount
    // Los filósofos pares toman primero el tenedor de su derecha, los impares el de su izquierda
    waitSem(MUTEX_SEM);
    int firstFork = philosopher % 2 == 0 ? rightFork(philosopher) : leftFork(philosopher);
    int secondFork = philosopher % 2 == 0 ? leftFork(philosopher) : rightFork(philosopher);
    philosophers[philosopher].state = HUNGRY;
    postSem(MUTEX_SEM);

    takeFork(philosopher, firstFork);
    waitSem(MUTEX_SEM);
    if (philosopher % 2 == 0)
    {
        philosophers[philosopher].hasRightFork = 1;
    }
    else
    {
        philosophers[philosopher].hasLeftFork = 1;
    }
    postSem(MUTEX_SEM);
    takeFork(philosopher, secondFork);
    waitSem(MUTEX_SEM);
    if (philosopher % 2 == 0)
    {
        philosophers[philosopher].hasLeftFork = 1;
    }
    else
    {
        philosophers[philosopher].hasRightFork = 1;
    }
    postSem(MUTEX_SEM);

    waitSem(MUTEX_SEM);
    philosophers[philosopher].state = EATING;
    displayTableState();
    postSem(MUTEX_SEM);
}

static void takeFork(int philosopher, int fork)
{
    int amFirst = 0;
    insertLast(philosWaitingForFork[fork], philosopher);
    while (!amFirst)
    {
        waitSem(fork);
        amFirst = (peekFirst(philosWaitingForFork[fork]) == philosopher);
        if (!amFirst)
        {
            postSem(fork);
        }
    }
    removeFirst(philosWaitingForFork[fork]);
}

static void putDownForks(int philosopher)
{
    waitSem(MUTEX_SEM);
    int firstFork = philosopher % 2 == 0 ? rightFork(philosopher) : leftFork(philosopher);
    int secondFork = philosopher % 2 == 0 ? leftFork(philosopher) : rightFork(philosopher);

    philosophers[philosopher].state = THINKING;
    displayTableState();

    postSem(firstFork);
    postSem(secondFork);
    philosophers[philosopher].hasLeftFork = 0;
    philosophers[philosopher].hasRightFork = 0;
    postSem(MUTEX_SEM);
}

static void displayTableState()
{
    for (int i = 0; i < currentPhilosopherAmount; i++)
    {
        printf("%s ", philosophers[i].state == EATING ? "E" : ".");
    }
    printf("\n");
}

// Think very hard
static void think()
{
    wait(THINKING_TICKS);
}

static void eat()
{
    wait(EATING_TICKS);
}

static int leftFork(int philosopher)
{
    return philosopher;
}

static int rightFork(int philosopher)
{
    return (philosopher + 1) % currentPhilosopherAmount;
}