#include "lexico.h"
#include "TS.h"
#include "definiciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern FILE* archivo;
extern ComponenteLexico* raizTabla;

ComponenteLexico* procesarComentario(int c);
ComponenteLexico* procesarComentarioMulti(int c);
ComponenteLexico* procesarCadenaAlfanumerica(int c);
ComponenteLexico* procesarNumero(int c);
ComponenteLexico* procesarOperacion(int c);
ComponenteLexico* procesarDelimitador(int c);

ComponenteLexico* siguienteComponenteLexico(){

    //Empezamos a leer caracter a caracter, sólo me face falta leer uno y pasárselo a un autómata
    int c;
    while ((c = fgetc(archivo)) != EOF) {
        if(c == '#'){
            return procesarComentario(c);
        }
        else if(c == '"'){
            return procesarComentarioMulti(c);
        }
        else if((c >= 65 && c <= 90) || (c >= 97 && c <= 122)){
            return procesarCadenaAlfanumerica(c);
        }
        else if(c >= 48 && c <= 57){
            return procesarNumero(c);
        }
        else if(c == '+' || c == '-' || c == '/' || c == '*' || c == '^' || c == '=' || c == '<' || c == '>' || c == '%'){
            return procesarOperacion(c);
        }
        else if(c == '{' || c == '(' || c == '[' || c == '.' || c == ',' || c == ':'){
            return procesarDelimitador(c);
        }
    }

}

ComponenteLexico* procesarComentario(int c){
    //Para procesar un comentario, simplemente leo caracteres hasta encontrar un retorno de carro
    int d;
    while ((d = fgetc(archivo)) != EOF) {
        if(d == '\n'){
            ComponenteLexico* a = crearNodo("\n", NEWLINE);
            return a;
        }
    }
}

ComponenteLexico* procesarComentarioMulti(int c){
    //Para procesar un comentario multi linea, primero miro que haya otras 2 comillas
    int d;
    //Buffer para comprobar comentario
    char buf_start[3];
    char buf_end[3];
    int i = 0;
    int j = 0;
    buf_start[i] = c;

    while ((d = fgetc(archivo)) != EOF) {
        if(d == '"' && i < 2){
            buf_start[++i] = d;
        }
        else if(d == '"' && i >= 2){
            buf_end[j] = d;
            j++;
        }
        else if(d != '"' && j > 0 && j < 3){
            j = 0;
        }

        if(j == 3){
            ComponenteLexico* a = crearNodo("\n", NEWLINE);
            return a;
        }
    }
}

ComponenteLexico* procesarCadenaAlfanumerica(int c){
    //Hay que leer hasta un delimitador específico: puntos, espacios en blanco o saltos de linea
    int d;

    char buffer[20];
    int i = 1;
    buffer[0] = c;

    while ((d = fgetc(archivo)) != EOF) {
        if(d == '\n' || d == '.' || d == ' ' || d == '(' || d == '[' || d == '{'){
            ungetc(d, archivo);
            break;
        }
        buffer[i] = d;
        i++;
    }

    //Primero Hay que buscar en la tabla para ver si es palabra reservada o no
    buffer[i] = '\0';
    ComponenteLexico* a;
    a = buscar(raizTabla, buffer);
    //Si no se encuentra, hay que añadirlo y es un ID
    if(a == NULL)
    {
        a = crearNodo(buffer, ID);
        raizTabla = insertarNodo(raizTabla, a->lexema, a->valor);
    }
    return a;
}

ComponenteLexico* procesarNumero(int c){
    int d;
    char buffer[20];
    if(c == '0'){
        if(d = fgetc(archivo) != EOF){
            //De acuerdo con la especificación de Python, puedo tener numeros octales, hexadecimales y binarios
            //Hago un sub-autómata para cada uno
            switch (d)
            {
            case 'b':
                enteroBinario(buffer);
                break;
            case 'x':
                enteroHexadecimal(buffer);
                break;
            case 'o':
                enteroOctal(buffer);
                break;
            case 'e':
                floatExponencial(buffer);
                break;
            default:
                break;
            }
        }
    }
    //TODO ESTA PARTE NO ESTA HECHA AUN
    else{
        if(d = fgetc(archivo) != EOF){
            //De acuerdo con la especificación de Python, puedo tener floats normales o con exponencial
            //Lo que pasa es que si no recibo la e, o bien tengo que llegar a un punto para determinar que es un float
            //O bien no llego a un punto nunca y entonces es un entero
            switch (d)
            {
            case 'e':
                floatExponencial(buffer);
                break;
            default:
                enteroDecimal(buffer, d);
                break;
            }
        }
    }
}

void enteroBinario(char* buffer){
    int d, i = 0;
    while(d = fgetc(archivo) != EOF){
        if(d == '0' || d == '1'){
            buffer[i] = d;
            i++;
        }
        else{
            buffer[i] = '\0';
            break;
        }
    }
}

void enteroOctal(char* buffer){
    int d, i = 0;
    while(d = fgetc(archivo) != EOF){
        if(d >= 48 && d <= 55){
            buffer[i] = d;
            i++;
        }
        else{
            buffer[i] = '\0';
            break;
        }
    }
}

void enteroHexadecimal(char* buffer){
    int d, i = 0;
    while(d = fgetc(archivo) != EOF){
        if((d >= 48 && d <= 57) || (d >= 97 && d <= 102)){
            buffer[i] = d;
            i++;
        }
        else{
            buffer[i] = '\0';
            break;
        }
    }
}

//TODO ESTE NO ESTA HECHO AUN
void enteroDecimal(char* buffer, int d){
    int i = 0;

    while(d = fgetc(archivo) != EOF){
        if((d >= 48 && d <= 57) || (d >= 97 && d <= 102)){
            buffer[i] = d;
            i++;
        }
        else{
            buffer[i] = '\0';
            break;
        }
    }
}

ComponenteLexico* procesarDelimitador(int c){
    int d;
    while ((d = fgetc(archivo)) != EOF) {
        if(d == '\n'){
            ComponenteLexico* a = crearNodo("\n", NEWLINE);
            return a;
        }
    }
}
