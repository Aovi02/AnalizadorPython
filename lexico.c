#include "lexico.h"
#include "TS.h"
#include "definiciones.h"
#include "entrada.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern ComponenteLexico* raizTabla;

ComponenteLexico* procesarComentario(int c);
ComponenteLexico* procesarComentarioMulti(int c);
ComponenteLexico* procesarCadenaAlfanumerica(int c);
ComponenteLexico* procesarNumero(int c);
ComponenteLexico* procesarOperacion(int c);
ComponenteLexico* procesarDelimitador(int c);

ComponenteLexico* siguienteComponenteLexico(){

    //Empezamos a leer caracter a caracter, sólo me face falta leer uno y pasárselo a un autómata
    char* p = siguienteCaracter(1);
    char c = *p;

    while(c == '\n' || c == ' ' || c == '\t'){
        p = siguienteCaracter(0);
        c = *p;
    }

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
    else if(c == '{' || c == '(' || c == '[' || c == '.' || c == ',' || c == ':' || c == '}' || c == ')' || c == ']'){
        return procesarDelimitador(c);
    }

}

ComponenteLexico* procesarComentario(int c){
    //Para procesar un comentario, simplemente leo caracteres hasta encontrar un retorno de carro
    char* p;
    while ((p = siguienteCaracter(0)) != NULL) {
        c = *p;
        if(c == '\n'){
            ComponenteLexico* a = crearNodo("\n", NEWLINE);
            return a;
        }
    }
    return NULL;
}

ComponenteLexico* procesarComentarioMulti(int c){
    //Para procesar un comentario multi linea, primero miro que haya otras 2 comillas
    char* p;
    //Buffer para comprobar comentario
    char buf_start[3];
    char buf_end[3];
    int i = 0;
    int j = 0;
    buf_start[i] = c;

    while ((p = siguienteCaracter(0)) != NULL) {
        c = *p;
        if(c == '"' && i < 2){
            buf_start[++i] = c;
        }
        else if(c == '"' && i >= 2){
            buf_end[j] = c;
            j++;
        }
        else if(c != '"' && j > 0 && j < 3){
            j = 0;
        }

        if(j == 3){
            ComponenteLexico* a = crearNodo("\n", NEWLINE);
            return a;
        }
    }
    return NULL;
}

ComponenteLexico* procesarCadenaAlfanumerica(int c){
    //Hay que leer hasta un delimitador específico: puntos, espacios en blanco o saltos de linea
    char* p;

    char buffer[TAM_BLOQUE];
    int i = 1;
    buffer[0] = c;

    while ((p = siguienteCaracter(1)) != NULL) {
        c = *p;
        if(c == '\n' || c == '.' || c == ' ' || c == '(' || c == '[' || c == '{'){
            devolverCaracter();
            break;
        }
        buffer[i] = c;
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

}

ComponenteLexico* procesarDelimitador(int c){
    char buffer[1];
    buffer[0] = c;
    ComponenteLexico* a = crearNodo(buffer, DELIMITADOR);
    return a;
}

ComponenteLexico* procesarOperacion(int c){
    //Comprobar los diferentes tipos de operaciones
    char buffer[3];
    buffer[0] = c;

    //Vamos a coger otro char
    char* p = siguienteCaracter(1);
    c = *p;

    switch (c)
    {
    case '=':
        //El igual es el último siempre, entonces aquí son operadores de 2 caracteres
        buffer[1] = c;
        buffer[2] = '\0';
        ComponenteLexico* a;
        //Aquí puede haber un montón de símbolos que precedan al igual, tengo que ver cuál de ellos es
        switch (buffer[0])
        {
        case '+':
            a = crearNodo(buffer, MAS_IGUAL);
            break;
        case '-':
            a = crearNodo(buffer, MENOS_IGUAL);
            break;
        case '*':
            a = crearNodo(buffer, MULT_IGUAL);
            break;
        case '/':
            a = crearNodo(buffer, DIV_IGUAL);
            break;
        case '=':
            a = crearNodo(buffer, IGUAL_IGUAL);
            break;
        case '>':
            a = crearNodo(buffer, MAYOR_IGUAL);
            break;
        case '<':
            a = crearNodo(buffer, MENOR_IGUAL);
            break;
        case '^':
            a = crearNodo(buffer, ELEVAR_IGUAL);
            break;
        case '%':
        a = crearNodo(buffer, MODULO_IGUAL);
            break;
        default:
            break;
        }
        return a;
    //Los casos a partir de aqui son de 3 caracteres y no hay ninguno en el wilcoxon
    case '/':
        break;
    case '*':
        break;
    case '>':
        break;
    case '<':
        break;
    default:
        //Es una operación de único carácter
        buffer[1] = '\0';
        devolverCaracter();
        a = crearNodo(buffer, c);
        return a;
    }
}
