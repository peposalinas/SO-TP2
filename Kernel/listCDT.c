// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include "./include/listADT.h"
#include "./include/MemoryManagerADT.h"

// Función para crear un nuevo nodo
static Node *create_node(void *data)
{
    Node *new_node = (Node *)allocMemoryKernel(sizeof(Node));
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

// Crear una nueva lista enlazada
LinkedList *createLinkedList()
{
    LinkedList *list = (LinkedList *)allocMemoryKernel(sizeof(LinkedList));
    list->head = NULL;
    list->size = 0;
    return list;
}

// Insertar un elemento al principio
void insertFirst(LinkedList *list, void *data)
{
    Node *new_node = create_node(data);
    new_node->next = list->head;
    list->head = new_node;
    list->size++;
}

// Insertar un elemento al final
void insertLast(LinkedList *list, void *data)
{
    Node *new_node = create_node(data);
    if (list->head == NULL)
    {
        list->head = new_node;
    }
    else
    {
        Node *temp = list->head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_node;
    }
    list->size++;
}

// Eliminar un elemento de la lista
void *removeElem(LinkedList *list, void *data, bool (*compare)(void *, void *))
{
    if (list->head == NULL)
        return NULL;

    Node *temp = list->head;
    Node *prev = NULL;

    while (temp != NULL && !compare(temp->data, data))
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL)
        return NULL; // El elemento no fue encontrado

    if (prev == NULL)
    {
        // Si el nodo a eliminar es el primero
        list->head = temp->next;
    }
    else
    {
        prev->next = temp->next;
    }

    void *removedElement = temp->data;
    freeMemoryKernel(temp);
    list->size--;
    return removedElement;
}

// Buscar un elemento en la lista
void *findElem(LinkedList *list, void *data, bool (*compare)(void *, void *))
{
    Node *temp = list->head;
    while (temp != NULL)
    {
        if (compare(temp->data, data))
        {
            return temp->data;
        }
        temp = temp->next;
    }
    return NULL; // No se encontró el elemento
}

// Obtener el tamaño de la lista
int getSize(LinkedList *list)
{
    return list->size;
}

// Destruir la lista y liberar la memoria
void destroyLinkedList(LinkedList *list, void (*free_data)(void *))
{
    Node *temp = list->head;
    while (temp != NULL)
    {
        Node *next = temp->next;
        if (free_data != NULL)
        {
            free_data(temp->data); // Libera la memoria de los datos si es necesario
        }
        freeMemoryKernel(temp); // Libera el nodo
        temp = next;
    }
    freeMemoryKernel(list); // Libera la lista
}

void *removeFirst(LinkedList *list)
{
    if (list->head == NULL)
    {
        return NULL; // Si la lista está vacía, no hay nada que devolver
    }

    Node *node_to_remove = list->head; // Guardamos el nodo que vamos a eliminar
    void *data = node_to_remove->data; // Guardamos el dato que vamos a devolver

    list->head = list->head->next;    // Actualizamos la cabeza de la lista
    freeMemoryKernel(node_to_remove); // Liberamos la memoria del nodo, pero no de los datos

    list->size--; // Reducimos el tamaño de la lista
    return data;  // Devolvemos el dato
}
