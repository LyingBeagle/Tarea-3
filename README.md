
# Tarea 3 Estructura de Datos

**Descripcion**

La aplicacion es una gestion de libros, en la cual se procesa palabra a palabra que tiene el libro.

## Compilar y ejecutar codigo
### Desde Replit (Recomendado)
* Abra su cuenta en repl.it (si no tiene cree una a través de su cuenta en github)
* En el home de repl.it busque la tarea y clónela para comenzar a trabajar, en caso de no encontrarla inserte `https://github.com/LyingBeagle/Tarea-3.git` donde le pide la URL 
* Seleccione el lenguaje (C)
* Una vez importado, escriba `gcc *.c -o main -lm` para configurar la complacion
* Una vez guardado, presione el boton **Run** para probar el programa
### Desde sistema (No Recomendado)
---
* Descarge e instale [git](https://git-scm.com/downloads)
* Entre a `cmd` en su sistema
* Clone el repositorio con `git clone https://github.com/LyingBeagle/Tarea-3.git`
* Navege al directorio con `cd Tarea-3`
* Compile el codigo con `gcc *.c -o programa`
* Ejecute la aplicacion con `/programa.exe`
`No es recomendable a menos que se use en linux, ya que posee una libreria que no se encuentra en Windows`

---
## Funciones

### Principales

**Funcionando correctamente**

* Registro de multiples libros a traves de una carpeta
* Importacion de un libro a traves de un archivo
* Muestra de todos los libros registrados
* Busqueda de libros a travez de palabras en sus titulos
* Muestra de las palabras mas frecuentes de un libro

**Con Problemas**
* Muestra de palabras relevantes de un libro `No se calcula correctamente la relevancia`
-Posible razon `Cuando se calcula la relevancia, al quitar los ceros este deja un valor no verdadero, haciendo que no esten en la posicion correcta en el monticulo`

* Mostrar el contexto de una palabra `No se muestra correctamente las palabras `
-Posible razon `Puede que sea al guardar las posiciones en la importacion de las palabras por el ftell o esta mal implementado el fseek`


### Secundarias

**Funcionando correctamente**

* Muestra de un menu con opciones
* El proceso que se realiza al elegir una opciones
* Liberacion de memoria

## Nota 
El programa cuenta con una blacklist pre-hecha, pero puede ser modificada si lo desea.
