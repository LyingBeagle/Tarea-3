#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct HashMap HashMap;

typedef struct Pair {
     char * key;
     void * value;
} Pair;

/*
    Crea un mapa vacio
*/

HashMap * createMap(long capacity);

/*
    Se inserta un par clave-valor en el mapa. Si la clave ya existe,
    se actualiza el valor
*/

void insertMap(HashMap * table, char * key, void * value);

/*
    Elimina el par clave-valor asociado a la clave
*/

void eraseMap(HashMap * table, char * key);

/*
    Busca en el mapa la clave especificada y devuelve el valor asociado.
    Si la clave no se encuentra, se devuelve un valor nulo o produce excepcion
*/

Pair * searchMap(HashMap * table, char * key);

/*
    Devuelve un puntero al primer par clave-valor en el mapa.
*/

Pair * firstMap(HashMap * table);

/*
    Devuelve el puntero siguiente a firstMap o de un anterior nextMap
*/

Pair * nextMap(HashMap * table);

/*
    Aumenta el tamaño del mapa
*/

void enlarge(HashMap * map);

/*
    Devuelve la ultima pair disponible del mapa
*/

Pair * getLastPair(HashMap * map);

/*
    Delvuelve el pair anterior 
*/

Pair * getMapPreviousPair(HashMap * map);

/*
    Libera la memoria del mapa y sus elementos
*/

void deleteMap(HashMap * map);

#endif