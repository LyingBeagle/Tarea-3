#include "hashmap.h"
#include "funciones.h"
#include "heap.h"
#include "list.h"
#include <ctype.h>
#include <dirent.h> //Esta funcion no es estandar, pero dijieron que la podia usar 
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Tamaño default para mapas
#define DEFAULT_MAP 100

struct Libro {
  char *titulo;                //Titulo del libro
  char *ID;                    //ID del libro
  HashMap *palabrasEnLibro;    //Mapa con las palabras del libro
  Heap *palabrasEnHeap;        //Monticulo de las palabras con su cantidad de prioridad
  int caracteres;              //Cantidad de caracteres del libro
  int cantPalabras;            //Cantidad de palabras del libro
  char* ruta;                  //Ruta de donde se encuentra el archivo del libro
};

struct Palabra {
  char *palabra;               //La palabra
  int cantPalabra;             //Cantidad de veces que se encuentra la palabra en el libro
  List* posiciones;            //Lista de posiciones de la palabra en el libro
};


int main(void) {
  menu();
  return 0;
}

//Menu
void menu() {

  int opcion = 0;

  //Mapas para guardar los libros, uno por ID y otro por nombre
  HashMap *bibliotecaPorID = createMap(DEFAULT_MAP);
  HashMap *bibliotecaPorNombre = createMap(DEFAULT_MAP);

  do {

    printf("\nBienvenido a la busqueda de documentos\n");
    printf("---------------------------------\n");
    printf("\n1.Cargar Documentos");
    printf("\n2.Mostrar Documentos");
    printf("\n3.Buscar libros con palabras en sus titulos");
    printf("\n4.Mostrar palabras más frecuentes de un libro");
    printf("\n5.Mostrar palabras más relevantes de un libro");
    printf("\n6.Buscar libros que contengan una palabra");
    printf("\n7.Mostrar palabra en contexto");
    printf("\n8.Salir\n");
    printf("\nOpcion: ");
    scanf(" %d", &opcion);

    // Se envia la opcion y los mapas para ser procesados
    proceso(opcion, bibliotecaPorID, bibliotecaPorNombre);

  } while (opcion != 8);
}

//Proceso
void proceso(int opcion, HashMap *bibliotecaPorID,
             HashMap *bibliotecaPorNombre) {

  switch (opcion) {

  case 1:
    cargaDocumentos(bibliotecaPorID, bibliotecaPorNombre);
    break;
  case 2:
    mostrarLibros(bibliotecaPorID);
    break;
  case 3:
    buscarLibrosPorPalabras(bibliotecaPorID);
    break;
  case 4:
    mostrarPalabrasMasFrecuentes(bibliotecaPorID);
    break;
  case 5:
    mostrarPalabrasMasRelevantes(bibliotecaPorNombre);
    break;
  case 6:
    buscarPorPalabra(bibliotecaPorID);
    break;
  case 7:
    mostrarPalabraEnContexto(bibliotecaPorNombre);
    break;
  case 8:
    printf("\nAdios\n");
    //Se libera la memoria
    liberarMemoria(bibliotecaPorID);
    free(bibliotecaPorNombre);
    break;
  }
}

// Opcion 1
void cargaDocumentos(HashMap *bibliotecaPorID, HashMap *bibliotecaPorNombre) {

  //Se crea un mapa con palabras que NO se guardaran
  HashMap *mapaBlacklist = blacklist_words(); 

  //Se obtiene el nombre de la carpeta que guarda los documentos
  char carpeta[1024];
  printf("\nIngrese el nombre de la carpeta:\n");
  scanf(" %1023[^\n]", carpeta);
  
  // Obtén el directorio actual
  char directorio_actual[256];

  //Se obtiene la direccion donde se encuentra el programa
  if (getcwd(directorio_actual, sizeof(directorio_actual)) == NULL) {
    perror("Error al obtener el directorio actual");
    return;
  }

  // Concatena la carpeta con la ruta del directorio actual
  char ruta_completa[256];
  snprintf(ruta_completa, sizeof(ruta_completa), "%s/%s", directorio_actual,
           carpeta);

  //Se abre el directorio
  DIR *dir = opendir(ruta_completa);
  if (dir == NULL) {
    perror("Error al abrir la carpeta");
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    //Se revisa que el archivo sea uno regular
    if (entry->d_type == DT_REG) {
      //Se une la ruta completa con el nombre del archivo
      char ruta_archivo[256];
      snprintf(ruta_archivo, sizeof(ruta_archivo), "%s/%s", ruta_completa,
               entry->d_name);
      FILE *archivo = fopen(ruta_archivo, "r");
      if (archivo == NULL) {
        perror("Error al abrir archivo");
        return;
      }
      //Se importa el libro
      importarLibro(archivo, ruta_archivo, (char *)entry->d_name, bibliotecaPorID,
                    mapaBlacklist, bibliotecaPorNombre);
      fclose(archivo);
    }
  }

  //Se borra el mapa de la blacklist y se cierra el directorio
  deleteMap(mapaBlacklist);
  closedir(dir);
}

void importarLibro(FILE *archivo,char* ruta_archivo, char *id_libro, HashMap *bibliotecaPorID,
                   HashMap *mapaBlacklist, HashMap *bibliotecaPorNombre) {

  //Linea que se usara para obtener el titulo y borrar hasta que se empieze el libro
  char linea[1024];

  // Guardar la posición actual en el archivo
  long int posicion = 0;

  //Se obtiene la primera linea
  fgets(linea, sizeof(linea), archivo);

  //Se elimina el encabezado
  eliminarEncabezado(linea);

  //Se elimina al autor
  char *token = strstr(linea, ", by");

  if (token != NULL) {
    // Encontramos ", by" en la línea
    *token = '\0'; // Terminamos la cadena en la posición de ", by"
    token += 5;    // Avanzamos el puntero para omitir ", by"
  }

  //Se reemplaza el salto de linea
  linea[strcspn(linea, "\n")] = '\0';

  //Se inicializa el libro
  Libro *libro = inicializarLibro(linea,ruta_archivo, id_libro);

  //Se borra lineas de introduccion del archivo hasta que empieze el libro
  while (fgets(linea, sizeof(linea), archivo) != NULL) {
    if (strstr(linea, "*** START OF"))
      break;
  }

  //Se procesa palabra a palabra del archivo
  char *palabra = siguiente_palabra(archivo);

  while (palabra) {
    //Se obtiene la posicion de la palabra
    posicion = ftell(archivo);
    //Se procesa la palabra
    procesarPalabra(palabra, libro, mapaBlacklist,posicion);
    palabra = siguiente_palabra(archivo);
  }

  //Se pasa el mapa de palabras a un monticulo
  mapaPasaMonticulo(libro);

  //Se inserta el libro a ambos mapas
  insertMap(bibliotecaPorID, libro->ID, libro);
  insertMap(bibliotecaPorNombre, libro->titulo, libro);
}

//Se crea el mapa de blacklist de palabras
HashMap *blacklist_words() {

  //Se crea el mapa
  HashMap *blacklistWords = createMap(DEFAULT_MAP);

  FILE *archivo = fopen("blacklist.txt", "r");

  if (archivo == NULL) {
    perror("Error al abrir archivo");
    return NULL;
  }

  //Se procesar palabra a palabra
  char *palabra = siguiente_palabra(archivo);

  while (palabra) {
    //Se inserta al mapa blacklistWords
    insertMap(blacklistWords, palabra, palabra);
    palabra = siguiente_palabra(archivo);
  }

  fclose(archivo);
  return blacklistWords; //Se retorna el mapa
}

//Se elimina una parte del encabezado
void eliminarEncabezado(char *linea) {

  //Los tres tipos de encabezado
  char encabezadoA[] = "The Project Gutenberg eBook of ";
  char encabezadoB[] = "Project Gutenberg’s ";
  char encabezadoC[] = "The Project Gutenberg EBook of ";

  char *inicio = strstr(linea, encabezadoA); // Buscar el encabezado en la línea

  //Se borra de la linea
  if (inicio != NULL) {
    size_t longitud_encabezado = strlen(encabezadoA);
    memmove(linea, inicio + longitud_encabezado,
            strlen(inicio + longitud_encabezado) + 1);
  } else if ((inicio = strstr(linea, encabezadoB)) != NULL) {
    size_t longitud_encabezado = strlen(encabezadoB);
    memmove(linea, inicio + longitud_encabezado,
            strlen(inicio + longitud_encabezado) + 1);
  } else if ((inicio = strstr(linea, encabezadoC)) != NULL) {
    size_t longitud_encabezado = strlen(encabezadoC);
    memmove(linea, inicio + longitud_encabezado,
            strlen(inicio + longitud_encabezado) + 1);
  }
}

//Se inicializa un libro
Libro *inicializarLibro(char *autor,char *ruta_archivo, char *id_libro) {

  //Se crea el libro
  Libro *libro = (Libro *)malloc(sizeof(Libro));

  if (libro == NULL) {
    printf("\nNo se pudo crear el libro\n");
    return NULL;
  }

  //Se asignan los atributos
  libro->titulo = strdup(autor);

  libro->ID = strtok(strdup(id_libro), ".");

  libro->ruta = strdup(ruta_archivo);

  libro->palabrasEnLibro = createMap(DEFAULT_MAP);

  libro->cantPalabras = 0;

  libro->caracteres = 0;

  return libro; //Se retorna
}

//Se lee el archivo y pasa a la siguiente palabra
char *siguiente_palabra(FILE *archivo) {

  char palabra[1024];

  if (fscanf(archivo, " %1023s", palabra) == 1) {
    return strdup(palabra);
  } else {
    return NULL;
  }
}

//Se quitan los caracteres de una cadena
char *quitar_caracteres(char *string, char *c) {

  int i, j;
  for (i = 0; i < strlen(string); i++) {

    //Se revisa si en esa posicion esta los caracteres a borrar
    if (hayQueEliminar(string[i], c)) {
      for (j = i; j < strlen(string) - 1; j++) {
        string[j] = string[j + 1];
      }
      string[j] = '\0';
      i--;
    }
  }
  return string;
}

//Se revisa si un caracter es igual a uno en la cadena
int hayQueEliminar(char c, char *string_chars) {

  for (int i = 0; i < strlen(string_chars); i++)
    if (string_chars[i] == c)
      return 1;

  return 0;
}

//Se procesa la palabra
void procesarPalabra(char *palabra, Libro *libro, HashMap *mapaBlacklist, long int posicion) {

  //Se le quitan los caracteres especificos a la palabra
  palabra = quitar_caracteres(palabra, ".,;:!?—-_\"\'’“”()[]{} *");
  //Se pasa a lowercase
  for (int i = 0; palabra[i]; i++) {
    palabra[i] = tolower(palabra[i]);
  }

  // Verificar si la palabra está en la lista negra
  if (searchMap(mapaBlacklist, palabra) == NULL) {

    //Se buscar la palabra en el mapa de su libro
    Pair *pairPalabra = searchMap(libro->palabrasEnLibro, palabra);

    if (pairPalabra == NULL) {
      //Si no la encuentra la inicializa y la añade al mapa
      Palabra *palabraEstructura = (Palabra *)malloc(sizeof(Palabra));
      palabraEstructura->palabra = strdup(palabra);
      palabraEstructura->cantPalabra = 1;
      palabraEstructura->posiciones = createList();
      insertMap(libro->palabrasEnLibro, palabraEstructura->palabra,
                palabraEstructura);
    } else {
      //Si la encuentra le aumenta la cantidad y guarda su posicion
      Palabra *palabraEstructura = (Palabra *)pairPalabra->value;
      palabraEstructura->cantPalabra++;
      pushBack(palabraEstructura->posiciones, &posicion);
    }

    //Se aumenta la cantidad de palabras y caracteres
    libro->cantPalabras++;
    libro->caracteres += sizeof(palabra);
  }
}

//Se pasa del mapa de palabras a un monticulo
void mapaPasaMonticulo(Libro *libro) {

  //Se buscar la primera palabra del mapa
  Pair *current = firstMap(libro->palabrasEnLibro);
  Heap *wordHeap = createHeap(); // Crear un montículo binario para almacenar las palabras


  while (current != NULL) {
    Palabra *palabraEstructura = (Palabra *)current->value;

    if (palabraEstructura == NULL) {
      continue;
    }

    // Almacenar la palabra y como prioridad su cantidad
    heap_push(wordHeap, palabraEstructura, palabraEstructura->cantPalabra);

    current = nextMap(libro->palabrasEnLibro);
  }

  // Asignar el heap de palabras al libro
  libro->palabrasEnHeap = wordHeap;
}

// Opcion 2
void mostrarLibros(HashMap *biblioteca) {
  
  // Se recorre la biblioteca y se imprimen los libros
  Pair *current = firstMap(biblioteca);

  if (current == NULL) {
    printf("\nNo se encontro Libros registrados\n");
    return;
  }

  while (current != NULL) {

    Libro *libro = (Libro *)current->value;

    printf("\nTitulo: %s", libro->titulo);
    printf("\nID: %s", libro->ID);
    printf("\nCantidad de palabras: %d", libro->cantPalabras);
    printf("\nCantidad de caracteres: %d", libro->caracteres);

    printf("\n");

    current = nextMap(biblioteca);
  }
}

// Opcion 3
void buscarLibrosPorPalabras(HashMap *biblioteca) {

  //Se crea una lista de para los libros con la palabra
  List *listaLibros = createList();

  char palabras[256];

  printf("\nIngrese palabras claves separadas por espacios: \n");
  scanf(" %255[^\n]", palabras);

  char palabras_copia[256];
  strncpy(palabras_copia, palabras, sizeof(palabras_copia));

  //Se crea un token para que guarde las multiples palabras
  char *token = strtok(palabras_copia, " ");

  
  for (Pair *current = firstMap(biblioteca); current != NULL;
       current = nextMap(biblioteca)) {

    Libro *libro = (Libro *)current->value;

    int todasPresentes = 1; // Variable para saber si todas las palabras fueron encontradas

    while (token != NULL) {

      if (strstr(libro->titulo, token) == NULL) {
        todasPresentes = 0;  //Una palabra NO fue encontrada
        break;
      }

      token = strtok(NULL, " "); //Se pasa a la siguiente palabra
    }

    //Se reinicia el token
    strncpy(palabras_copia, palabras, sizeof(palabras_copia));
    token = strtok(palabras_copia, " ");

    //Si todas las palabras se encontraron, añade el libro a la lista
    if (todasPresentes) {
      pushBack(listaLibros, libro);
    }
  }

  Node *currentList = firstList(listaLibros);

  if (currentList == NULL) {
    printf("\nNo se encontraron libros con titulos que tengan esas palabras\n");
    return;
  }

  printf("\nLibros que coinciden con las palabras\n");

  //Se revisa toda la lista y se imprimen los libros
  while (currentList != NULL) {

    Libro *libro = (Libro *)currentList;

    printf("\n%s", libro->titulo);

    currentList = nextList(listaLibros);
  }

  printf("\n");

  //Se borra la lista
  cleanList(listaLibros);
  free(listaLibros);
}

// Opcion 4
void mostrarPalabrasMasFrecuentes(HashMap *biblioteca) {

  //Se busca el libro
  char id[50];
  int contPalabras = 0; //Contador para las 10 palabras

  printf("\nIngrese el ID del libro\n");
  scanf(" %49[^\n]", id);

  Pair *pairLibro = searchMap(biblioteca, id);

  if (pairLibro == NULL) {
    printf("\nNo se encontro el libro\n");
    return;
  }

  Libro *libro = (Libro *)pairLibro->value;

  // Crear una lista temporal para almacenar las 10 palabras más frecuentes
  List *top10Palabras = createList();

  // Obtener los 10 elementos más frecuentes del montículo
  for (int i = 0; i < 10; i++) {
    if (emptyHeap(libro->palabrasEnHeap)) {
      break; // Salir si no hay más elementos en el montículo
    }

    //Se guardan los elementos del monticulo
    void *top = heap_top(libro->palabrasEnHeap);
    Palabra *palabraEstructura = (Palabra *)top;
    heap_pop(libro->palabrasEnHeap);

    if (palabraEstructura != NULL) {
      pushBack(top10Palabras, palabraEstructura);
    }
  }

  // Mostrar las 10 palabras más frecuentes
  Node *currentList = firstList(top10Palabras);

  printf("\nLas 10 palabras con más frecuencia son\n");
  printf("\n");

  while (currentList != NULL) {
    Palabra *palabraEstructura = (Palabra *)currentList;

    if (palabraEstructura != NULL) {
      printf("Palabra: %s\n", palabraEstructura->palabra);
      printf("Cantidad de apariciones: %d\n", palabraEstructura->cantPalabra);
      printf("Frecuencia: %.10lf%%\n",
             (double)(palabraEstructura->cantPalabra) / libro->cantPalabras);

      printf("\n");
      currentList = nextList(top10Palabras);
    }
  }

  // Reinsertar los elementos en el montículo
  currentList = firstList(top10Palabras);

  while (currentList != NULL) {
    Palabra *palabraEstructura = (Palabra *)currentList;

    if (palabraEstructura != NULL) {
      heap_push(libro->palabrasEnHeap, palabraEstructura,
                palabraEstructura->cantPalabra);
      currentList = nextList(top10Palabras);
    }
  }

  // Liberar la lista temporal
  cleanList(top10Palabras);
  free(top10Palabras);
}

// Opcion 5
void mostrarPalabrasMasRelevantes(HashMap *bibliotecaPorNombre) {

  //Se guarda el titulo
  char cadena[1024];
  int contPalabras = 0; //Variable para las 10 palabras

  printf("\nIngrese el nombre del libro\n");
  scanf(" %1023[^\n]", cadena);

  char *titulo = strdup(cadena);

  //Se busca en el mapa
  Pair *pairLibro = searchMap(bibliotecaPorNombre, titulo);

  if (pairLibro == NULL) {
    printf("\nNo se encontro el libro\n");
    return;
  }

  Libro *libro = (Libro *)pairLibro->value;

  //Se crea lista para guardar las palabras
  List *top10Palabras = createList();

  if(top10Palabras == NULL){
    return;
  }

  //Se guardan las 10 palabras mas frecuentes
  Heap *palabrasRelevancia = crearMonticuloPorRelevancia(bibliotecaPorNombre, libro);

  if(palabrasRelevancia == NULL){
    printf("\nNo se encontraron palabras relevantes\n");
    return;
  }

  // Obtener los 10 elementos más frecuentes del montículo
  for (int i = 0; i < 10; i++) {
    if (emptyHeap(palabrasRelevancia)) {
      break; // Salir si no hay más elementos en el montículo
    }

    void *top = heap_top(palabrasRelevancia);
    Palabra *palabraEstructura = (Palabra *)top;
    heap_pop(palabrasRelevancia);

    if (palabraEstructura != NULL) {
      pushBack(top10Palabras, palabraEstructura);
    }
  }

  // Mostrar las 10 palabras más frecuentes
  Node *currentList = firstList(top10Palabras);

  printf("\nLas 10 palabras con más relevancia son\n");
  printf("\n");

  while (currentList != NULL) {
    Palabra *palabraEstructura = (Palabra *)currentList;

    if (palabraEstructura != NULL) {

      printf("Palabra: %s\n", palabraEstructura->palabra);

      printf("\n");
      currentList = nextList(top10Palabras);
    }
  }

  // Reinsertar los elementos en el montículo
  currentList = firstList(top10Palabras);

  while (currentList != NULL) {
    Palabra *palabraEstructura = (Palabra *)currentList;

    if (palabraEstructura != NULL) {
      heap_push(libro->palabrasEnHeap, palabraEstructura,
                palabraEstructura->cantPalabra);
      currentList = nextList(top10Palabras);
    }
  }
  free(titulo);
  free(palabrasRelevancia);
}

//Se crea un monticulo con relevancia como prioridad
Heap *crearMonticuloPorRelevancia(HashMap *bibliotecaPorNombre, Libro *libro) {

  // Crear un montículo vacío
  Heap *relevanciaHeap = createHeap();

  Pair *pairPalabra = firstMap(libro->palabrasEnLibro);

  if (pairPalabra == NULL)
    return NULL;

  //Se revisa palabra a palabra calculando su relevancia y añadiendola al monticulo
  while (pairPalabra != NULL) {

    Palabra *palabra = (Palabra *)pairPalabra->value;

    if (palabra != NULL) {

      //Se guarda la relevancia de la palabra
      int relevancia = eliminarCeroDecimales(calcularRelevancia(palabra, libro, bibliotecaPorNombre));

      heap_push(relevanciaHeap, palabra, relevancia);
    }
    pairPalabra = nextMap(libro->palabrasEnLibro);
  }

  return relevanciaHeap; //Se retorna el monticulo
}

//Calculo de la relevancia de una palabra
double calcularRelevancia(Palabra* palabra, Libro* libro, HashMap* biblioteca){

    return ((double)(palabra->cantPalabra) / libro->cantPalabras) *
        log10(cantidadDocumentos(biblioteca) /
              cantidadAparicionesPalabraEnLibros(biblioteca,
                                                 palabra->palabra));
}

//Se calcula la cantidad de apariciones de una palabra en todos los libros
int cantidadAparicionesPalabraEnLibros(HashMap *biblioteca, char *palabra) {

  //Variable para la cantidad de apariciones de la palabra en los libros
  int cantApariciones = 0; 

  Pair *pairLibro = firstMap(biblioteca);

  if (pairLibro == NULL) {
    return 0;
  }

  while (pairLibro != NULL) {

    Libro *libro = (Libro *)pairLibro->value;

    //Se busca la palabra
    Pair *pairPalabra = searchMap(libro->palabrasEnLibro, palabra);

    if (pairPalabra != NULL) {
      cantApariciones++; //Si la encuentra, aumenta la cantidad de apariciones
    }

    pairLibro = nextMap(biblioteca);
  }

  return cantApariciones; //Retorna la cantiadad de apariciones
}

//Se calcula la cantidad de libros en registrados
int cantidadDocumentos(HashMap *biblioteca) {

  int cantidad = 0;

  Pair *current = firstMap(biblioteca);

  while (current != NULL) {
    cantidad++;
    current = nextMap(biblioteca);
  }

  return cantidad;
}

//Se eliminan los ceros de un decimal
int eliminarCeroDecimales(double num){

    while(num - floor(num) > 0){
      num *= 10;
    }

    return (int)num;
}

//Opcion 6
void buscarPorPalabra(HashMap *bibliotecaPorID) {

    //Se pregunta por la palabra a buscar en los libros
    char palabra[1024];

    printf("\nIngrese la palabra que desea buscar en los libros:\n");
    scanf(" %1023[^\n]", palabra);

    // Crear una lista para almacenar libros
    List *listaLibrosConPalabra = createList();

    Pair *pairLibro = firstMap(bibliotecaPorID);

    //Se busca la palabra en todos los libros
    while (pairLibro != NULL) {
        Libro *libro = (Libro *)pairLibro->value;
        Pair *pairPalabra = searchMap(libro->palabrasEnLibro, palabra);

        if (pairPalabra != NULL) {
            Palabra *estructuraPalabra = (Palabra *)pairPalabra->value;

            pushBack(listaLibrosConPalabra, libro);
        }

        pairLibro = nextMap(bibliotecaPorID);
    }

    //Se muestran todos los libros que la poseean
    Node* current = firstList(listaLibrosConPalabra);

    if(current == NULL){
      printf("\nNo se encontraron libros con la palabra ingresada.\n");
      return;
    }

    printf("\nLibros que contienen la palabra\n");
    printf("\n");

    while(current != NULL){

        Libro* libro = (Libro*)current;

        printf("Título: %s\n", libro->titulo);
        printf("ID: %s\n\n", libro->ID);

        current = nextList(listaLibrosConPalabra);
    }

    // Liberar la memoria utilizada por la lista
    free(listaLibrosConPalabra);
}

//Opcion 7
void mostrarPalabraEnContexto(HashMap* bibliotecaPorNombre) {

    //Se pregunta y busca el libro
    char titulo[1024];
    printf("\nIngrese el titulo del libro:\n");
    scanf(" %1023[^\n]", titulo);

    Pair* pairLibro = searchMap(bibliotecaPorNombre, titulo);
    
    if (pairLibro == NULL) {
        printf("\nLibro no encontrado.\n");
        return;
    }

    Libro* libro = (Libro*)pairLibro->value;

    char palabra[1024];
    printf("\nIngrese la palabra a buscar:\n");
    scanf(" %1023[^\n]", palabra);
  
    Pair *pairPalabra = searchMap(libro->palabrasEnLibro, palabra);

    if (pairPalabra == NULL) {
        printf("La palabra no se encontró en el libro.\n");
        return;
    }
  
    Palabra *estructuraPalabra = (Palabra *)pairPalabra->value;

    printf("Ocurrencias de la palabra '%s' en el libro '%s':\n", palabra, libro->titulo);

    // Se obtiene la lista de posiciones de la palabra
    void *posiciones = firstList(estructuraPalabra->posiciones);

    FILE* archivo = fopen(libro->ruta, "r"); //Se abre el archivo del libro

    if(archivo == NULL){
      printf("\nNo se pudo abrir el archivo\n");
      return;
    }

    //Se muestran 10 palabras antes y despues de la palabra buscada
    while (posiciones != NULL) {
        int posOriginal = *(int*)posiciones;
        int posicion = posOriginal - 5;
        fseek(archivo, posicion, SEEK_CUR);

        for(int i = 0; i < 10; i++){
          char contexto[1024];
          fscanf(archivo, " %1023s", contexto);
           printf("%s ", contexto);
        }
        
        printf("\n");
       
        posiciones = nextList(estructuraPalabra->posiciones);
    }
    fclose(archivo);
}

//Se libera la memoria del mapa y las palabras
void liberarMemoria(HashMap *mapa) {
  // Liberar la memoria de la biblioteca
  Pair *current = firstMap(mapa);
  while (current != NULL) {
    Libro *libro = (Libro *)current->value;
    // Liberar el mapa de palabras de cada libro
    if (firstMap(libro->palabrasEnLibro) != NULL) {
      liberarPalabras(libro->palabrasEnLibro);
      deleteMap(libro->palabrasEnLibro);
    }
    free(libro->titulo);
    free(libro->ID);
    free(libro->ruta);
    free(libro);
    current = nextMap(mapa);
  }
  // Liberar la biblioteca
  deleteMap(mapa);
}

void liberarPalabras(HashMap *palabrasEnLibro) {

  Pair *pairPalabra = firstMap(palabrasEnLibro);

  while (pairPalabra != NULL) {

    Palabra* palabraEsctructura = (Palabra*)pairPalabra;

    free(palabraEsctructura->palabra);

    pairPalabra = nextMap(palabrasEnLibro);
  }
}
