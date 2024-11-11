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
static void takeForks(int philosopher, int firstFork, int secondFork);
static void takeFork(int philosopher, int fork);
static void putDownForks(int philosopher, int firstFork, int secondFork);
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
    waitSemMutex(MUTEX_SEM);
    for (int i = 0; i < currentPhilosopherAmount; i++)
    {
        killProcess(philosophers[i].pid);
        closeSem(i);
        destroyLinkedList(philosWaitingForFork[i]);
    }
    postSemMutex(MUTEX_SEM);
    closeSem(MUTEX_SEM);
    exitProc(0);
    return 0;
}

static void addPhilo()
{
    waitSemMutex(MUTEX_SEM);
    // char numberStr[MAX_NUMBER_CHARS];
    if (currentPhilosopherAmount == MAX_PHILOSOPHERS)
    {
        printf("No se pueden agregar más filosofos\n");
        postSemMutex(MUTEX_SEM);
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
    postSemMutex(MUTEX_SEM);
}

static void removePhilo()
{
    waitSemMutex(MUTEX_SEM);
    if (currentPhilosopherAmount == 0)
    {
        printf("No hay filosofos para remover\n");
        postSemMutex(MUTEX_SEM);
        return;
    }
    else if (currentPhilosopherAmount == MIN_PHILOSOPHERS)
    {
        printf("No se pueden remover mas filosofos\n");
        postSemMutex(MUTEX_SEM);
        return;
    }
    int toRemove = currentPhilosopherAmount - 1;

    while (philosophers[toRemove - 1].state != THINKING)
    {
        postSemMutex(MUTEX_SEM);
        waitSemMutex(toRemove);
        waitSemMutex(MUTEX_SEM);
    }

    killProcess(philosophers[toRemove].pid);
    closeSem(toRemove);
    destroyLinkedList(philosWaitingForFork[toRemove]);

    removeElem(philosWaitingForFork[0], toRemove);
    postSemMutex(0);

    currentPhilosopherAmount--;
    postSemMutex(MUTEX_SEM);
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

        waitSemMutex(MUTEX_SEM);
        int firstFork = philosopher % 2 == 0 ? rightFork(philosopher) : leftFork(philosopher);
        int secondFork = philosopher % 2 == 0 ? leftFork(philosopher) : rightFork(philosopher);
        philosophers[philosopher].state = HUNGRY;
        postSemMutex(MUTEX_SEM);

        takeForks(philosopher, firstFork, secondFork);
        eat();
        putDownForks(philosopher, firstFork, secondFork);
    }
    exitProc(1);
    return 1;
}

static void takeForks(int philosopher, int firstFork, int secondFork)
{
    // A la izquierda del filósofo i, está el tenedor i. A su derecha, el tenedor i+1%currentPhilosopherAmount
    // Los filósofos pares toman primero el tenedor de su derecha, los impares el de su izquierda

    takeFork(philosopher, firstFork);
    takeFork(philosopher, secondFork);

    waitSemMutex(MUTEX_SEM);
    philosophers[philosopher].state = EATING;
    displayTableState();
    postSemMutex(MUTEX_SEM);
}

static void takeFork(int philosopher, int fork)
{
    int amFirst = 0;
    insertLast(philosWaitingForFork[fork], philosopher);
    while (!amFirst)
    {
        waitSemMutex(fork);
        amFirst = (peekFirst(philosWaitingForFork[fork]) == philosopher);
        if (!amFirst)
        {
            postSemMutex(fork);
        }
    }
    removeFirst(philosWaitingForFork[fork]);
}

static void putDownForks(int philosopher, int firstFork, int secondFork)
{
    waitSemMutex(MUTEX_SEM);
    philosophers[philosopher].state = THINKING;
    displayTableState();

    postSemMutex(firstFork);
    postSemMutex(secondFork);
    philosophers[philosopher].hasLeftFork = 0;
    philosophers[philosopher].hasRightFork = 0;
    postSemMutex(MUTEX_SEM);
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