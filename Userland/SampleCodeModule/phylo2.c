#include "libc.h"

#define MAX_QTY 8
#define MIN_QTY 3
#define MUTEX_SEM_ID 42
#define MAX_PHILO_NUMBER_BUFFER 3

#define EAT_TIME 1
#define THINK_TIME 1

#define COMMAND_QUIT 'q'
#define COMMAND_ADD 'a'
#define COMMAND_REMOVE 'r'
#define COMMAND_CLEAR 'c'

#define LEFT(i) (((i) + philosophersAmount - 1) % philosophersAmount)
#define RIGHT(i) (((i) + 1) % philosophersAmount)
#define PHILOSOPHER_SEM(i) (MUTEX_SEM_ID + (i) + 1)

typedef enum
{
    NONE = 0,
    EATING,
    HUNGRY,
    THINKING
} philosopher_state;

static uint8_t philosophersAmount = 0;
static philosopher_state philosophersStates[MAX_QTY];
static int16_t philosopherPids[MAX_QTY];
static uint8_t singleLine = 0;

static void render();
static int philosopher(int argc, char **argv);
static int8_t addPhilosopher(int index);
static int8_t removePhilosopher(int index);
static void takeBothForks(int i);
static void putBothForks(int i);
static void test(int i);

char *argvs2[MAX_QTY][1] = {{"0"}, {"1"}, {"2"}, {"3"}, {"4"}, {"5"}, {"6"}, {"7"}};

void reverse2(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void intToArray(int num, char result[], int buffer_size)
{
    if (num == 0)
    {
        result[0] = '0';
        result[1] = '\0';
        return;
    }

    int is_negative = 0;

    if (num < 0)
    {
        is_negative = 1;
        num = -num;
    }

    int i = 0;
    while (num != 0)
    {
        int rem = num % 10;
        result[i++] = rem + '0';
        num = num / 10;
    }

    if (is_negative)
    {
        result[i++] = '-';
    }

    result[i] = '\0';

    reverse2(result, i);
}

int runPhilosophers(int argc, char **argv)
{
    printf("Bienvenido a la cena de los filosofos!\nPresione 'q' para salir, 'a' para agregar un filosofo y 'r' para remover uno\n");
    philosophersAmount = 0;
    singleLine = 0;
    if (openSem(MUTEX_SEM_ID, 1) == -1)
    {
        return -1;
    }

    for (int i = 0; i < MAX_QTY; i++)
    {
        philosophersStates[i] = NONE;
        philosopherPids[i] = -1;
    }

    for (int i = 0; i < MIN_QTY + 2; i++)
    {
        addPhilosopher(i);
    }

    char command = '\0';
    uint8_t aux;
    while ((command = getChar(&aux)) != COMMAND_QUIT)
    {
        switch (command)
        {
        case COMMAND_ADD:
            if (philosophersAmount < MAX_QTY)
            {
                if (addPhilosopher(philosophersAmount) == -1)
                    printf("No se pudo agregar al filosofo\n");
            }
            else
                printf("La mesa no da para mas filosofos\n");
            break;
        case COMMAND_REMOVE:
            if (philosophersAmount > MIN_QTY)
                removePhilosopher(philosophersAmount - 1);
            else
                printf("No puede haber menos de 3 filosofos\n");
            break;
        case COMMAND_CLEAR:
            singleLine = !singleLine;
            break;
        }
    }
    closeSem(MUTEX_SEM_ID);
    for (int i = 0; i < philosophersAmount; i++)
    {
        closeSem(MUTEX_SEM_ID + 1 + i);
    }
    exitProc(0);
    return 0;
}

static void
render()
{
    const static char letters[] = {' ', 'E', '.', '.'};
    uint8_t something_to_write = 0;
    for (int i = 0; i < philosophersAmount; i++)
    {
        if (letters[philosophersStates[i]] != ' ')
        {
            something_to_write = 1;
            if (philosophersStates[i] == EATING)
            {
                printf("G ");
            }
            else
            {
                printf(". ");
            }
        }
    }
    if (something_to_write)
        printf("\n");
}

static int8_t
addPhilosopher(int index)
{
    waitSem(MUTEX_SEM_ID);
    char philo_number_buffer[MAX_PHILO_NUMBER_BUFFER] = {0};
    if (openSem(PHILOSOPHER_SEM(index), 0) == -1)
    {
        return -1;
    }
    intToArray(index, philo_number_buffer, 10);

    philosopherPids[index] = createStandardProc("philosopher", philosopher, 2, argvs2[index]);
    if (philosopherPids[index] != -1)
    {
        philosophersAmount++;
    }
    render();
    postSem(MUTEX_SEM_ID);
    return -1 * !(philosopherPids[index] + 1);
}

static int8_t
removePhilosopher(int index)
{
    printf("El filósofo %d ha comido demasiado y se retira\n", philosopherPids[index]);

    waitSem(MUTEX_SEM_ID);

    while (philosophersStates[LEFT(index)] == EATING && philosophersStates[RIGHT(index)] == EATING)
    {
        postSem(MUTEX_SEM_ID);
        waitSem(PHILOSOPHER_SEM(index));
        waitSem(MUTEX_SEM_ID);
    }

    killProcess(philosopherPids[index]);

    closeSem(PHILOSOPHER_SEM(index));
    philosopherPids[index] = -1;
    philosophersStates[index] = NONE;
    philosophersAmount--;
    postSem(MUTEX_SEM_ID);
    render();
    return 0;
}

static int
philosopher(int argc, char **argv)
{
    int i = atoi(argv[0]);
    printf("Soy el filosofo %d y estoy listo para comer una rica pasta!\n", philosopherPids[i]);
    philosophersStates[i] = THINKING;
    while (1)
    {
        wait(THINK_TIME);
        takeBothForks(i);
        wait(EAT_TIME);
        putBothForks(i);
    }
    return 0;
}

static void
takeBothForks(int i)
{
    waitSem(MUTEX_SEM_ID);
    philosophersStates[i] = HUNGRY;
    test(i);
    postSem(MUTEX_SEM_ID);
    waitSem(PHILOSOPHER_SEM(i));
}

static void
putBothForks(int i)
{
    waitSem(MUTEX_SEM_ID);
    philosophersStates[i] = THINKING;
    render();
    test(LEFT(i));
    test(RIGHT(i));
    postSem(MUTEX_SEM_ID);
}

static void
test(int i)
{
    if (philosophersStates[i] == HUNGRY && philosophersStates[LEFT(i)] != EATING &&
        philosophersStates[RIGHT(i)] != EATING)
    {
        philosophersStates[i] = EATING;
        render();
        postSem(PHILOSOPHER_SEM(i));
    }
}