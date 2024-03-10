#include "TS.h"
#include "lexico.h"
#include "sintactico.h"
#include "definiciones.h"
#include "entrada.h"
#include <stdio.h>

FILE* archivo;
ComponenteLexico* raizTabla;

void iniciarTabla();

int main(){

    //Abro el archivo si no está ya abierto y manejo el error asociado de abrirlo
    if(archivo == NULL){
        archivo = fopen("wilcoxon.py", "r");
        if(archivo == NULL){
            perror("Error al abrir el archivo");
            return -1;
        }
    }
    //Iniciamos el sistema de entrada
    iniciarEntrada();

    //Iniciamos tabla de símbolos
    iniciarTabla();

    //Iniciamos analizador sintactico
    iniciarSintactico();

    return 0;
    
}

void iniciarTabla(){
    raizTabla = crearNodo("import", IMPORT);
    raizTabla = insertarNodo(raizTabla, "as", AS);
    raizTabla = insertarNodo(raizTabla, "def", DEF);
    raizTabla = insertarNodo(raizTabla, "for", FOR);
    raizTabla = insertarNodo(raizTabla, "in", IN);
    raizTabla = insertarNodo(raizTabla, "if", IF);
    raizTabla = insertarNodo(raizTabla, "elif", ELIF);
    raizTabla = insertarNodo(raizTabla, "else", ELSE);
    raizTabla = insertarNodo(raizTabla, "return", RETURN);
    raizTabla = insertarNodo(raizTabla, "not", NOT);
}