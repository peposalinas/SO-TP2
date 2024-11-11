#include "libc.h"
#include "list.h"

// Definimos la estructura de un nodo
typedef struct Node
{
    int data;
    struct Node *next;
} Node;

// Definimos la estructura de la lista enlazada
struct LinkedList
{
    Node *head;
    int size;
};

// Función para crear un nuevo nodo
static Node *create_node(int data)
{
    Node *new_node = allocM(sizeof(Node));
    if (new_node == NULL)
    {
        printf("Error al asignar memoria para el nodo.\n");
        return NULL;
    }
    new_node->data = data;
    new_node->next = NULL;
    return new_node;
}

// Crear una nueva lista enlazada
LinkedList *createLinkedList()
{
    LinkedList *list = allocM(sizeof(LinkedList));
    if (list == NULL)
    {
        printf("Error al asignar memoria para el nodo.\n");
        return NULL;
    }
    list->head = NULL;
    list->size = 0;
    return list;
}

// Insertar un elemento al principio
void insertFirst(LinkedList *list, int data)
{
    Node *new_node = create_node(data);
    new_node->next = list->head;
    list->head = new_node;
    list->size++;
}

// Insertar un elemento al final
void insertLast(LinkedList *list, int data)
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
void removeElem(LinkedList *list, int data)
{
    if (list->head == NULL)
        return;

    Node *temp = list->head;
    Node *prev = NULL;

    while (temp != NULL && temp->data != data)
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

    freeM(temp);
    list->size--;
}

// Buscar un elemento en la lista
int findElem(LinkedList *list, int data)
{
    Node *temp = list->head;
    while (temp != NULL)
    {
        if (temp->data == data)
        {
            return temp->data;
        }
        temp = temp->next;
    }
    return -1; // No se encontró el elemento
}

// Obtener el tamaño de la lista
int getSize(LinkedList *list)
{
    return list->size;
}

// Destruir la lista y liberar la memoria
void destroyLinkedList(LinkedList *list)
{
    Node *temp = list->head;
    while (temp != NULL)
    {
        Node *next = temp->next;
        freeM(temp); // Libera el nodo
        temp = next;
    }
    freeM(list); // Libera la lista
}

int removeFirst(LinkedList *list)
{
    if (list->head == NULL)
    {
        return -1; // Si la lista está vacía, no hay nada que devolver
    }

    Node *node_to_remove = list->head; // Guardamos el nodo que vamos a eliminar
    int data = node_to_remove->data;   // Guardamos el dato que vamos a devolver

    list->head = list->head->next; // Actualizamos la cabeza de la lista
    freeM(node_to_remove);         // Liberamos la memoria del nodo, pero no de los datos

    list->size--; // Reducimos el tamaño de la lista
    return data;  // Devolvemos el dato
}

int peekFirst(LinkedList *list)
{
    if (list->head == NULL)
    {
        return -1; // Si la lista está vacía, no hay nada que devolver
    }

    return list->head->data; // Devolvemos el dato del primer nodo
}