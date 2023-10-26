#ifndef Heap_h
#define Heap_h

typedef struct Heap Heap;

/*
    Creacion de un monticulo
*/

Heap* createHeap();

/*
    Se retorna la raiz  del monticulo
*/

void* heap_top(Heap* pq);

/*
    Se añade un elemento al monticulo
*/

void heap_push(Heap* pq, void* data, int priority);

/*
    Se elimina la raiz del monticulo
*/

void heap_pop(Heap* pq);

/*
    Se revisa si el monticulo esta vacio
*/
int emptyHeap(Heap* pq);


#endif

