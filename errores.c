#include "errores.h"
#include <stdio.h>

void errorLexemaGrande(){
    printf("\nERROR: Lexema demasiado grande.\n");
}

void errorLiteralMalformado(int linea){
    printf("\nERROR: Literal mal formado en la linea: %d\n\n", linea);
}

void errorFloatMalformado(int linea){
    printf("\nERROR: Float mal formado en la linea: %d\n\n", linea);
}

void errorEnteroMalformado(int linea){
    printf("\nERROR: Entero mal formado en la linea: %d\n\n", linea);
}

void errorLexemaNoReconocido(int linea){
    printf("\nERROR: Lexema no reconocido en la linea: %d\n\n", linea);
}

void errorAperturaArchivo(){
    printf("\nERROR: No se pudo abrir el archivo a procesar\n\n");
}
