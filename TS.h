
#include "lexico.h"
#ifndef TS_H
#define TS_H

/*
Esto va a ser un árbol binario normal, donde los nodos son componentes léxicos. El árbol se ordena por orden alfabético de lexemas
*/

//Iniciar el árbol, implica añadir todos los nodos necesarios para las palabras reservadas
ComponenteLexico* crearNodo(char* lexema, int valor);

//Función para insertar un elemento en el árbol, automáticamente decide si a la derecha o izquierda dependiendo del orden
ComponenteLexico* insertarNodo(ComponenteLexico* raiz, char* lexema, int valor);

//Función para buscar un elemento en el árbol
ComponenteLexico* buscar(ComponenteLexico* raiz, char* lexema);

//Función para borrar el arbol entero
void borrarArbol(ComponenteLexico* raiz);

//Imprimir el arbol
void recorridoInorden(ComponenteLexico* raiz);


#endif  // TS_H