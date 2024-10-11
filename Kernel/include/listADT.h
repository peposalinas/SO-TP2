#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>

// Definimos la estructura de un nodo
typedef struct Node
{
    void *data;
    struct Node *next;
} Node;

// Definimos la estructura de la lista enlazada
struct LinkedList
{
    Node *head;
    int size;
};

// Definimos el tipo LinkedList
typedef struct LinkedList LinkedList;

// Crear una nueva lista enlazada
LinkedList *createLinkedList();

// Insertar un elemento en la lista al principio
void insertFirst(LinkedList *list, void *data);

// Insertar un elemento en la lista al final
void insertLast(LinkedList *list, void *data);

// Eliminar un elemento de la lista (el primero que encuentre)
void *removeElem(LinkedList *list, void *data, bool (*compare)(void *, void *));

// Buscar un elemento en la lista
void *findElem(LinkedList *list, void *data, bool (*compare)(void *, void *));

// Obtener el tamaño de la lista
int getSize(LinkedList *list);

// Destruir la lista y liberar la memoria
void destroyLinkedList(LinkedList *list, void (*free_data)(void *));

// Remueve el primero de la lista y lo devuelve
void *removeFirst(LinkedList *list);

#endif
