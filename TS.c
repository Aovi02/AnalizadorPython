#include "TS.h"
#include "lexico.h"
#include "definiciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ComponenteLexico* crearNodo(char* lexema, int valor){
    //Reservar memoria para el primer nodo
    ComponenteLexico* raiz = (ComponenteLexico*) malloc(sizeof(ComponenteLexico));
    //Configurar el nodo
    int len = strlen(lexema);
    raiz->lexema = (char*)malloc(len + 1);
    strncpy(raiz->lexema, lexema, len + 1);
    raiz->valor = valor;
    raiz->der = NULL;
    raiz->izq = NULL;

    return raiz;
}

ComponenteLexico* insertarNodo(ComponenteLexico* raiz, char* lexema, int valor){
    //Primero ver si la raíz no es nula, tendría que crearla y parar
    if(raiz == NULL){
        return crearNodo(lexema, valor);
    }

    //Una vez tenga la raíz, puedo insertar a la izquierda o derecha según el lexema
    int p = strcmp(lexema, raiz->lexema);

    //Si el lexema es menor, va antes, entonces a la izquierda
    //Esto es recursivo, se usa el nodo de la izquierda o de la derecha para llamar a esta función
    if(p < 0){
        raiz->izq = insertarNodo(raiz->izq, lexema, valor);
    }
    //Esto es lo mismo pero si el lexema es mayor, y va a la derecha
    else{
        raiz->der = insertarNodo(raiz->der, lexema, valor);
    }

    return raiz;
}

ComponenteLexico* buscar(ComponenteLexico* raiz, char* lexema) {
    //Si el lexema coincide con la raiz, no hace falta buscar mñás
    if (raiz == NULL) {
        return raiz;
    }
    int p = strcmp(raiz->lexema, lexema);

    if(p == 0)
        return raiz;

    //Buscar en el subárbol izquierdo de forma recursiva, es como la función de añadir, va de la misma forma
    //Se puede poner return aquí en los ifs porque la función parará cuando el primer if se cumpla
    //En un momento dado, algún nodo del subárbol se convertirá en el argumento raiz de esta función y coincidirá con el lexema
    if (p < 0) {
        return buscar(raiz->izq, lexema);
    }
    //Buscar en el subárbol derecho de forma recursiva
    else {
        return buscar(raiz->der, lexema);
    }
}

void borrarArbol(ComponenteLexico* raiz){
    //Esto es recursivo también, se llama a sí mismo hasta borrar todos los nodos, parará cuando la raiz sea null
    //En un momento dado se llegará a las hojas, que serán el argumento raíz de esta función y ya no tendrán hijos, los cuales serán nulos
    if(raiz != NULL){
        borrarArbol(raiz->izq);
        borrarArbol(raiz->der);
        free(raiz->lexema);
        free(raiz);
        raiz = NULL;
    }
}

// Function to perform an inorder traversal of the binary tree
void recorridoInorden(ComponenteLexico* raiz) {
    if (raiz != NULL) {
        recorridoInorden(raiz->izq);
        printf("[%s, %d] ", raiz->lexema, raiz->valor);
        recorridoInorden(raiz->der);
    }
}

