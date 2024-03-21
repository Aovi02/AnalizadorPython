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
            if(aux->valor == EOF){
                printf("\nFIN DEL ARCHIVO ENCONTRADO. FIN DEL ANALISIS\n");
                free(aux->lexema);
                free(aux);
                break;
            }
            if(!(aux->valor >= 300 && aux->valor <= 310)){
                free(aux->lexema);
                free(aux);
                aux = NULL;
            }
        }
    } while (1);
    
}