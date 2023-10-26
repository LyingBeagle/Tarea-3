#ifndef FUNCIONES_H
#define FUNCIONES_H

typedef struct Libro Libro;
typedef struct Palabra Palabra;

#include <stdio.h>
#include "hashmap.h"
#include "list.h"
#include "heap.h"

/*
    Menu para mostrar las opciones
*/

void menu();

/*
    Proceso que resive una opcion y la procesa
*/

void proceso(int, HashMap *, HashMap *);

/*
    Se cargan los documentos de una carpeta
*/

void cargaDocumentos(HashMap *, HashMap *);

/*
    De un archivo se importa un libro
*/

void importarLibro(FILE *,char*, char *, HashMap *, HashMap *, HashMap *);

/*
    Se retorna un mapa que tiene palabras que no se guardaran en el mapa de palabras
*/

HashMap *blacklist_words();

/*
    Se recive una linea y se revisa si tiene parte de un encabezado, si la tiene se borra
*/

void eliminarEncabezado(char *);

/*
    Se incializa un libro y se retorna
*/

Libro *inicializarLibro(char *,char*, char *);

/*
    Se recive un archivo y se pasa a la siguiente palabra que tenga, si no quedan retorna NULL
*/

char *siguiente_palabra(FILE *);

/*
    Se quitan caracteres especificos de una cadena, y se retorna modificada
*/

char *quitar_caracteres(char *, char *);

/*
    Se recive un caracter y una cadena, se retorna verdadero si lo encuentra en esta y falso sino
*/

int hayQueEliminar(char, char *);

/*
    Se procesa la palabra
*/

void procesarPalabra(char *, Libro *, HashMap *, long int);

/*
    Se pasa del mapa de palabras del libro a un monticulo
*/

void mapaPasaMonticulo(Libro *);

/*
    Se recorre el mapa y se muestran los libros
*/

void mostrarLibros(HashMap *);

/*
    Se muestran libros con palabras especificas en sus titulos
*/

void buscarLibrosPorPalabras(HashMap *);

/*
    Muestra las 10 palabras mas frecuentes de un libro por su ID
*/

void mostrarPalabrasMasFrecuentes(HashMap *);

/*
    Muestra las 10 palabras mas relevantes de un libro por su nombre
*/

void mostrarPalabrasMasRelevantes(HashMap *);

/*
    Se crea y retorna un monticulo que tiene como prioridad la relevancia
*/

Heap *crearMonticuloPorRelevancia(HashMap *, Libro *);

/*
    Se calcula la relevancia de una palabra
*/

double calcularRelevancia(Palabra*, Libro* , HashMap* );

/*
    Se calcula la cantidad de apariciones de una palabra en todos los libros
*/
int cantidadAparicionesPalabraEnLibros(HashMap *, char *);

/*
    Se calcula la cantidad de documentos registrados
*/

int cantidadDocumentos(HashMap *);

/*
    Se eliminan los ceros de un decimal
*/

int eliminarCeroDecimales(double);

/*
    Se buscar y muestra todos los libros que contengan cierta palabra
*/

void buscarPorPalabra(HashMap *);

/*
    Se muestran 10 palabras antes y despues de la palabra pedida
*/
void mostrarPalabraEnContexto(HashMap*);

/*
    Se libera la memoria de las bibliotecas
*/

void liberarMemoria(HashMap *);

/*
    Se libera la memoria de cada palabra
*/
void liberarPalabras(HashMap *);


#endif
