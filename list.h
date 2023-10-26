#ifndef LIST_H
#define LIST_H

typedef struct List List;
typedef struct Node Node;

struct Node {
    void * data;
    Node * next;
    Node * prev;
};

struct List {
    Node * head;
    Node * tail;
    Node * current;
};

/*
    Crea una nueva lista y la retorna
*/

List *createList();

/*
    Devuelve un puntero al primer elemento de la lista.
*/

void *firstList(List *);

/*
    Mueve el puntero de la lista al siguiente elemento y
    devuelve un puntero a dicho elemento.
*/

void *nextList(List *);

/*
    Devuelve un puntero al último elemento de la lista.
*/

void *lastList(List *);

/*
    Mueve el puntero de la lista al elemento anterior y
    devuelve un puntero a dicho elemento.
*/

void *prevList(List *);

/*
    Inserta un nuevo elemento al inicio de la lista
*/

void pushFront(List *, void *);

/*
    Inserta un nuevo elemento al final de la lista
*/

void pushBack(List *, void *);

/*
    Inserta un nuevo elemento en la posición actual de la lista
*/

void pushCurrent(List *, void *);

/*
    Elimina el primer elemento de la lista.
*/

void *popFront(List *);

/*
    Elimina el ultimo elemento de la lista
*/

void *popBack(List *);

/*
    Elimina el elemento acual de la lista
*/

void *popCurrent(List *);

/*
    Limpia toda la lista
*/

void cleanList(List *list);

/*
    Devuelve el tamaño de la lista
*/

int sizeList(List * list);

#endif