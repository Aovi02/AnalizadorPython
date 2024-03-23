#include "TS.h"
#include "lexico.h"
#include "sintactico.h"
#include "definiciones.h"
#include "entrada.h"
#include "errores.h"
#include <stdio.h>

//Variables para guardar el archivo y la raiz de la tabla (se llama raiz porque es un árbol binario, pero es básicamente la TS)
FILE* archivo;
ComponenteLexico* raizTabla;

void iniciarTabla();

int main(){

    //Abro el archivo si no está ya abierto y manejo el error asociado de abrirlo
    if(archivo == NULL){
        archivo = fopen("wilcoxon.py", "r");
        if(archivo == NULL){
            errorAperturaArchivo();
            return -1;
        }
    }
    //Iniciamos el sistema de entrada
    iniciarEntrada();

    //Iniciamos tabla de símbolos e imprimimos su estado
    iniciarTabla();
    printf("ESTADO DE LA TABLA DE SIMBOLOS TRAS INICIALIZACION:\n");
    recorridoInorden(raizTabla);
    printf("\n\n");

    //Iniciamos analizador sintactico
    iniciarSintactico();

    //Cuando acabe el análisis léxico, imprimimos el estado de la tabla de símbolos
    printf("ESTADO DE LA TABLA DE SIMBOLOS AL FINALIZAR:\n");
    recorridoInorden(raizTabla);
    printf("\n\n");

    //Borrar árbol al final
    borrarArbol(raizTabla);

    //Limpiar sistema de entrada
    limpiarEntrada();

    //Cerrar archivo
    fclose(archivo);

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