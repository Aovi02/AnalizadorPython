#include <stdio.h>
#include "sintactico.h"
#include "lexico.h"
#include "definiciones.h"

void iniciarSintactico(){
    ComponenteLexico* aux;
    do
    {
        aux = siguienteComponenteLexico();
        printf("<-- Componente: %d | Lexema: %s -->\n", aux->valor, aux->lexema);

    } while (aux->valor != EOF);
    
}