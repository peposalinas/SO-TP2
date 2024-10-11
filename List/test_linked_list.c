#include <stdio.h>
#include <stdlib.h>
#include "../Test/CuTest.h"
#include "listADTuser.h"

/*Correr: gcc -o tests ./List/listCDT.c ./List/test_linked_list.c ./Test/CuTest.c
Seguido de: ./tests
*/

// Función de comparación para enteros
bool compare_int(void *a, void *b)
{
    return *(int *)a == *(int *)b;
}

// Función para liberar enteros
void free_int(void *data)
{
    free(data);
}

// Test: Crear lista vacía
void TestCreateLinkedList(CuTest *tc)
{
    LinkedList *list = createLinkedList();
    CuAssertPtrNotNull(tc, list);
    CuAssertIntEquals(tc, 0, getSize(list));
    destroyLinkedList(list, free_int);
}

// Test: Insertar elemento en la lista (al principio)
void TestInsertFirst(CuTest *tc)
{
    LinkedList *list = createLinkedList();

    int *value = (int *)malloc(sizeof(int));
    *value = 10;
    insertFirst(list, value);

    CuAssertIntEquals(tc, 1, getSize(list));
    int *found = findElem(list, value, compare_int);
    CuAssertPtrNotNull(tc, found);
    CuAssertIntEquals(tc, 10, *found);

    destroyLinkedList(list, free_int);
}

// Test: Insertar elemento en la lista (al final)
void TestInsertLast(CuTest *tc)
{
    LinkedList *list = createLinkedList();

    int *value1 = (int *)malloc(sizeof(int));
    *value1 = 20;
    insertLast(list, value1);

    int *value2 = (int *)malloc(sizeof(int));
    *value2 = 30;
    insertLast(list, value2);

    CuAssertIntEquals(tc, 2, getSize(list));

    int *found1 = findElem(list, value1, compare_int);
    CuAssertIntEquals(tc, 20, *found1);

    int *found2 = findElem(list, value2, compare_int);
    CuAssertIntEquals(tc, 30, *found2);

    destroyLinkedList(list, free_int);
}

// Test: Eliminar elemento de la lista
void TestRemoveElem(CuTest *tc)
{
    LinkedList *list = createLinkedList();

    int *value1 = (int *)malloc(sizeof(int));
    *value1 = 40;
    insertLast(list, value1);

    int *value2 = (int *)malloc(sizeof(int));
    *value2 = 50;
    insertLast(list, value2);

    CuAssertIntEquals(tc, 2, getSize(list));

    // Eliminar el primer elemento (40)
    removeElem(list, value1, compare_int);
    CuAssertIntEquals(tc, 1, getSize(list));

    // Verificar que value1 no está
    int *found = findElem(list, value1, compare_int);
    CuAssertPtrEquals(tc, NULL, found);

    destroyLinkedList(list, free_int);
}

// Test: Remover el primer elemento de la lista
void TestRemoveFirst(CuTest *tc)
{
    LinkedList *list = createLinkedList();

    int *value1 = (int *)malloc(sizeof(int));
    *value1 = 100;
    insertLast(list, value1);

    int *value2 = (int *)malloc(sizeof(int));
    *value2 = 200;
    insertLast(list, value2);

    int *value3 = (int *)malloc(sizeof(int));
    *value3 = 300;
    insertLast(list, value3);

    // Verificar el tamaño inicial de la lista
    CuAssertIntEquals(tc, 3, getSize(list));

    // Remover el primer elemento (100)
    int *removed_value = (int *)removeFirst(list);
    CuAssertPtrNotNull(tc, removed_value);
    CuAssertIntEquals(tc, 100, *removed_value);
    free(removed_value); // Liberamos la memoria del dato removido

    // Verificar que el tamaño de la lista ha disminuido
    CuAssertIntEquals(tc, 2, getSize(list));

    // Verificar que el nuevo primer elemento es 200
    int *new_first = (int *)removeFirst(list);
    CuAssertIntEquals(tc, 200, *new_first);
    free(new_first);

    destroyLinkedList(list, free_int);
}

// Suite de tests
CuSuite *LinkedListGetSuite()
{
    CuSuite *suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, TestCreateLinkedList);
    SUITE_ADD_TEST(suite, TestInsertFirst);
    SUITE_ADD_TEST(suite, TestInsertLast);
    SUITE_ADD_TEST(suite, TestRemoveElem);
    SUITE_ADD_TEST(suite, TestRemoveFirst);
    return suite;
}

// Ejecutar todos los tests
void RunAllTests(void)
{
    CuString *output = CuStringNew();
    CuSuite *suite = CuSuiteNew();

    CuSuiteAddSuite(suite, LinkedListGetSuite());

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main(void)
{
    RunAllTests();
    return 0;
}
