#include <stdio.h>
#include <stdlib.h>
#include "listADTuser.h"

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

// Función para crear un nuevo nodo
static Node *create_node(void *data)
{
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (new_node == NULL)
    {
        fprintf(stderr, "Error al asignar memoria para el nodo.\n");
        exit(EXIT_FAILURE);
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

// Crear una nueva lista enlazada
LinkedList *createLinkedList()
{
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    if (list == NULL)
    {
        fprintf(stderr, "Error al asignar memoria para la lista.\n");
        exit(EXIT_FAILURE);
    }
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
void removeElem(LinkedList *list, void *data, bool (*compare)(void *, void *))
{
    if (list->head == NULL)
        return;

    Node *temp = list->head;
    Node *prev = NULL;

    while (temp != NULL && !compare(temp->data, data))
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL)
        return; // El elemento no fue encontrado

    if (prev == NULL)
    {
        // Si el nodo a eliminar es el primero
        list->head = temp->next;
    }
    else
    {
        prev->next = temp->next;
    }

    free(temp);
    list->size--;
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
        free(temp); // Libera el nodo
        temp = next;
    }
    free(list); // Libera la lista
}

void *removeFirst(LinkedList *list)
{
    if (list->head == NULL)
    {
        return NULL; // Si la lista está vacía, no hay nada que devolver
    }

    Node *node_to_remove = list->head; // Guardamos el nodo que vamos a eliminar
    void *data = node_to_remove->data; // Guardamos el dato que vamos a devolver

    list->head = list->head->next; // Actualizamos la cabeza de la lista
    free(node_to_remove);          // Liberamos la memoria del nodo, pero no de los datos

    list->size--; // Reducimos el tamaño de la lista
    return data;  // Devolvemos el dato
}
