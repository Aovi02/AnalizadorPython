#include <stdio.h>
#include "sintactico.h"
#include "lexico.h"
#include "definiciones.h"

void iniciarSintactico(){
    ComponenteLexico* aux;
    do
    {
        aux = siguienteComponenteLexico();
        printf("------------------------------------------------------\n");
        printf("Obtenido el siguiente componente lexico:\n");
        printf("Componente: %d\n", aux->valor);
        printf("Lexema: %s\n", aux->lexema);
        printf("------------------------------------------------------\n\n");

    } while (aux->valor != EOF);
    
}