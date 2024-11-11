#ifndef LINKEDLIST_H
#define LINKEDLIST_H

// Definimos el tipo LinkedList
typedef struct LinkedList LinkedList;

// Crear una nueva lista enlazada
LinkedList *createLinkedList();

// Insertar un elemento en la lista al principio
void insertFirst(LinkedList *list, int data);

// Insertar un elemento en la lista al final
void insertLast(LinkedList *list, int data);

// Eliminar un elemento de la lista (el primero que encuentre)
void removeElem(LinkedList *list, int data);

// Buscar un elemento en la lista
int findElem(LinkedList *list, int data);

// Obtener el tamaño de la lista
int getSize(LinkedList *list);

// Destruir la lista y liberar la memoria
void destroyLinkedList(LinkedList *list);

// Remueve el primero de la lista y lo devuelve
int removeFirst(LinkedList *list);

// Retorna el primer elemento sin removerlo
int peekFirst(LinkedList *list);

#endif
