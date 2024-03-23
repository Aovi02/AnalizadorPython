#include <stdio.h>
#include <stdlib.h>
#include "sintactico.h"
#include "lexico.h"
#include "definiciones.h"


void iniciarSintactico(){
    ComponenteLexico* aux;
    do
    {
        aux = siguienteComponenteLexico();
        if(aux != NULL){
            printf("--- Componente: %d | Lexema: \"%s\" ---\n", aux->valor, aux->lexema);
            //La única instancia en la que se ve a recibir un EOF es el final del archivo, por lo que se puede marcar
            if(aux->valor == EOF){
                printf("\nFIN DEL ARCHIVO ENCONTRADO. FIN DEL ANALISIS\n");
                free(aux->lexema);
                free(aux);
                break;
            }
            //Como siempre devuelvo un puntero, no puedo hacer free de los que están en la tabla de símbolos, pero al resto sí
            if(!(aux->valor >= 300 && aux->valor <= 310)){
                free(aux->lexema);
                free(aux);
                aux = NULL;
            }
        }
    } while (1);
    
}