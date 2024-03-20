#include "lexico.h"
#include "TS.h"
#include "definiciones.h"
#include "entrada.h"
#include "errores.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

extern ComponenteLexico* raizTabla;
char buffer[TAM_BLOQUE];
int posBuffer = 0;
char* p;
int linea = 1;

int esDelimitador(int c);
int esOperador(int c);
ComponenteLexico* procesarComentario(int c);
ComponenteLexico* procesarComentarioMulti(int c);
ComponenteLexico* procesarCadenaAlfanumerica(int c);
ComponenteLexico* procesarNumero(int c, int start);
ComponenteLexico* procesarOperacion(int c);
ComponenteLexico* procesarDelimitador(int c);
ComponenteLexico* procesarLiteral(int c, char comienzo);
ComponenteLexico* procesarFloatExponencial(int c);
ComponenteLexico* procesarNumeroHexadecimal(int c, int start);
ComponenteLexico* procesarFloat(int c, int start);

/*
Aquí es donde se pide un char y se decide a qué autómata se pasa dicho char
*/
ComponenteLexico* siguienteComponenteLexico(){

    //Empezamos a leer caracter a caracter
    p = siguienteCaracter(0);
    char c = *p;

    //Los espacios, los saltos de línea y las tabulaciones, así como el fin del archivo no importan, se ignoran
    while(c == '\n' || c == ' ' || c == '\t'){
        if(c == '\n')
            linea++;
        p = siguienteCaracter(0);
        c = *p;
    }

    //Esto es comentario de una sóla línea
    if(c == '#'){
        return procesarComentario(c);
    }
    else if(c == '"'){
        p = siguienteCaracter(1);
        c = *p;

        //Esto es un literal
        if(c != '"'){
            return procesarLiteral(c, '"');
        }
        //Esto es comentario multi-linea
        else{
            p = siguienteCaracter(1);
            c = *p;
            if(c == '"'){
                return procesarComentarioMulti(c);
            }   
        }
    }
    //Si es un literal de una sóla de comilla
    else if(c == '\''){
        p = siguienteCaracter(1);
        c = *p;

        //Esto es un literal
        if(c != '\''){
            return procesarLiteral(c, '\'');
        }
        //Esto es comentario multi-linea
        else{
            p = siguienteCaracter(1);
            c = *p;
            if(c == '\''){
                return procesarComentarioMulti(c);
            }   
        }
    }
    //Esto es cadena alfanumérica
    else if(isalpha(c) || c == '_'){
        return procesarCadenaAlfanumerica(c);
    }
    //Al encontrar un dígito, tengo que contemplar varios tipos de números
    else if(isdigit(c)){
        //Guardo el primer dígito
        buffer[0] = c;
        //Delantero está en - después de la llamada de abajo
        p = siguienteCaracter(1);
        c = *p;

        while(c == '\n' || c == ' ' || c == '\t'){
            if(c == '\n')
                linea++;
            p = siguienteCaracter(0);
            c = *p;
        }

        //PARTE FLOATS
        //Para los floats exponenciales (por ejemplo 5e-2)
        if(c == 'e'){
            buffer[1] = c;
            //Delantero está en 2 después de la llamada de abajo
            p = siguienteCaracter(1);
            c = *p;
            //Después de la e puede haber un - que tengo que considerar, si no lo hay ahora, no lo va a haber en ningún otro sitio
            if(c == '-' || isdigit(c)){
                return procesarFloat(c, 2);
            }
            //Si no es ni un dígito, ni un menos, el float está mal formado (quedaría 5e-)
            else{
                errorFloatMalformado(linea);
            }
        }
        //Para los floats que tienen punto justo después de un dígito
        else if(c == '.'){
            buffer[1] = c;
            //Delantero está en 2 después de la llamada de abajo
            p = siguienteCaracter(1);
            c = *p;
            //Sólo puede haber un dígito después de un punto (no se puede hacer cosas como 0.-6)
            if(isdigit(c)){
                return procesarFloat(c, 2);
            }
            else{
                errorFloatMalformado(linea);
            }
        }
        //PARTE ENTEROS
        //Si hay una x justo después de un 0, es hexadecimal
        else if(c == 'x' && buffer[0] == '0'){
            buffer[1] = c;
            p = siguienteCaracter(1);
            c = *p;
            //La referencia del lenguaje Python dice que tiene que haber un dígito más después de la x para ser válido, y como es hexadecimal, puede ser
            //también de A a F.
            if(isdigit(c) || (c >= 97 && c <= 102)){
                return procesarNumeroHexadecimal(c, 2);
            }
            else{
                errorEnteroMalformado(linea);
            }
        }
        //Si es un dígito a secas, es un entero y no puede haber otra cosa de por medio
        else if(isdigit(c)){
            return procesarNumero(c, 1);
        }
        //Si no es NINGUNO de los casos anteriores, devuelvo caracter y proceso lo que ya tengo en el buffer
        else{
            devolverCaracter();
            return procesarNumero(buffer[0], 0);
        }
    }
    //Procesar indicadores de operación, por si hay más símbolos después (para los que son += o ==)
    else if(esOperador(c)){
        return procesarOperacion(c);
    }
    //Procesar los delimitadores MENOS el punto
    else if(esDelimitador(c)){
        return procesarDelimitador(c);
    }
    //Para el punto, puede ser delimitador o un float que empiece por punto
    else if(c == '.'){
        buffer[0] = c;
        p = siguienteCaracter(1);
        c = *p;

        while(c == '\n' || c == ' ' || c == '\t'){
            if(c == '\n')
                linea++;
            p = siguienteCaracter(0);
            c = *p;
        }

        //Si el siguiente char es un dígito, es un float
        if(isdigit(c)){
            return procesarFloat(c, 1);
        }
        //Si no es un número, es un delimitador a secas
        else{
            devolverCaracter();
            return procesarDelimitador(buffer[0]);
        }
    }
    else if(c == EOF || c == '\005'){
        ComponenteLexico* a = crearNodo("\0", EOF);
        return a;
    }
    else{
        errorLexemaNoReconocido(linea);
    }
    return NULL;
}

//Función para comprobar que sea un delimitador, se usa dentro de otra funciones, así que por eso se incluye la función de si es operador
int esDelimitador(int c){
    return (c == '{' || c == '(' || c == '[' || c == ',' || c == ':' || c == '}' || c == ')' || c == ']') || esOperador(c);
}

int esOperador(int c){
    return (c == '+' || c == '-' || c == '/' || c == '*' || c == '^' || c == '=' || c == '<' || c == '>' || c == '%');
}

//Automáta para los comentarios de UNA SÓLA LÍNEA
ComponenteLexico* procesarComentario(int c){
    //Para procesar un comentario, leo caracteres hasta encontrar un retorno de carro
    while ((p = siguienteCaracter(0)) != NULL) {
        c = *p;
        if(c == '\n'){
            devolverCaracter();
            //Aquí devuelvo el retorno de carro para indicar que hubo comentario, pero no sería necesario
            ComponenteLexico* a = crearNodo("\n", NEWLINE);
            return a;
        }
    }
    return NULL;
}

//Autómata para procesar un literal string
ComponenteLexico* procesarLiteral(int c, char comienzo){
    buffer[0] = c;
    int i = 1;

    while ((p = siguienteCaracter(1)) != NULL) {
        c = *p;
        buffer[i] = c;
        if(c == comienzo){
            buffer[i] = '\0';
            ComponenteLexico* a = crearNodo(buffer, LITERAL);
            return a;
        }
        i++;
    }
    //Si en la última posición del bufer no hay '\0' es porque se ha leido hasta Cuenca y el lexema se ha pasado, entonces tengo que seguir leyendo ignorando caracteres
    //Eso significa que puedo crear el componenteLexico aquí, leer caracteres hasta que la máquina acepte, y después devolverlo. Si eso también falla, siempre queda el NULL
    if(buffer[i - 1] != '\0'){
        while ((p = siguienteCaracter(0)) != NULL) {
            c = *p;
            if(c == comienzo){
                buffer[i] = '\0';
                ComponenteLexico* a = crearNodo(buffer, LITERAL);
                return a;
            }
        }
    }
    return NULL;
}

//Autómata que procesa los comentarios multi linea
ComponenteLexico* procesarComentarioMulti(int c){

    //Lo único de lso comentarios es que si empiezan por comiilas triples, tienen que acabar en comillas triples y viceversa, por lo que hay que tenerlo en cuenta
    char comienzo = c;

    while ((p = siguienteCaracter(0)) != NULL) {
        c = *p;
        if(c == '"'){
            p = siguienteCaracter(0);
            c = *p;
            if(c == '"'){
                p = siguienteCaracter(0);
                c = *p;
                if(c == '"' && c == comienzo){
                    ComponenteLexico* a = crearNodo("\n", NEWLINE);
                    return a;
                }
            }
        }

        //Tengo que considerar el set de 3 comillas simples también, pero como tienen que ser seguidas, es repetir la estructura de antes
        if(c == '\''){
            p = siguienteCaracter(0);
            c = *p;
            if(c == '\''){
                p = siguienteCaracter(0);
                c = *p;
                if(c == '\'' && c == comienzo){
                    ComponenteLexico* a = crearNodo("\n", NEWLINE);
                    return a;
                }
            }
        }

    }
    return NULL;
}

//Autómata que procesa las cadenas alfanuméricas
ComponenteLexico* procesarCadenaAlfanumerica(int c){
    int i = 1;
    buffer[0] = c;

    while ((p = siguienteCaracter(1)) != NULL) {
        c = *p;
        if(isalnum(c) || c == '_'){
            buffer[i] = c;
            i++;
            continue;
        }
        //Se ignora el EOF del sistema de entrada
        //OJO; PUEDE DAR PROBLEMAS AL FINAL DEL ARCHIVO
        else if(c == EOF){
            continue;
        }
        //Paramos al encontrar un delimitador
        else{
            devolverCaracter();
            break;
        }
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

//Autómata que procesa números
ComponenteLexico* procesarNumero(int c, int start){
    buffer[start] = c;
    int i = start + 1;

    while ((p = siguienteCaracter(1)) != NULL) {
        c = *p;
        if(c != EOF){
            if(isdigit(c) || c == 'x'){
                buffer[i] = c;
                i++;
            }
            else{
                devolverCaracter();
                if (posBuffer <= TAM_BLOQUE - 1){
                    buffer[i] = '\0';
                }
                ComponenteLexico *a = crearNodo(buffer, NUMERO);
                return a;
            }
        }
    }
    return NULL;
}

//Autómata que procesa números hexadecimales
ComponenteLexico* procesarNumeroHexadecimal(int c, int start){
    buffer[start] = c;
    int i = start + 1;

    while ((p = siguienteCaracter(1)) != NULL) {
        c = *p;
        if(c != EOF){
            if(isdigit(c) || (c >= 97 && c <= 102)){
                buffer[i] = c;
                i++;
            }
            else{
                devolverCaracter();
                if (posBuffer <= TAM_BLOQUE - 1){
                    buffer[i] = '\0';
                }
                ComponenteLexico *a = crearNodo(buffer, NUMERO);
                return a;
            }
        }
    }
    return NULL;
}


//Autómata que procesa floats exponenciales
ComponenteLexico* procesarFloat(int c, int start){
    buffer[start] = c;
    int i = start + 1;
    while((p = siguienteCaracter(1)) != NULL){
        c = *p;
        if(c != EOF){
            if(isdigit(c)){
                buffer[i] = c;
                i++;
            }
            else{
                devolverCaracter();
                if (i <= TAM_BLOQUE - 1){
                    buffer[i] = '\0';
                }
                ComponenteLexico *a = crearNodo(buffer, NUMERO);
                return a;
            }
        }
    }
    return NULL;
}

//Autómata que procesa los delimitadores
ComponenteLexico* procesarDelimitador(int c){
    buffer[0] = c;
    buffer[1] = '\0';
    ComponenteLexico* a = crearNodo(buffer, c);
    posBuffer = 0;
    return a;
}

//Autómata que procesa las operaciones (en principio son delimitadores, pero queda más clara la diferencia si los separo)
ComponenteLexico* procesarOperacion(int c){
    //Comprobar los diferentes tipos de operaciones
    buffer[0] = c;

    //Vamos a coger otro char
    p = siguienteCaracter(1);
    c = *p;

    switch (c)
    {
    case '=':
        //El igual es el último siempre, entonces aquí son operadores de 2 caracteres
        buffer[1] = c;
        buffer[2] = '\0';
        ComponenteLexico* a;
        //Aquí puede haber un montón de símbolos que precedan al igual, tengo que ver cuál de ellos es y devolver el componente léxico asociado
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
    //Aquí vendrían operadores como **, //=, pero sólo el primero está en el wilcoxon
    case '/':
        break;
    case '*':
        //De momento es **
        if(buffer[0] == '*'){
            buffer[1] = c;
            buffer[2] = '\0';
            a = crearNodo(buffer, DOBLE_MULT);
            return a;
        }
        break;
    case '>':
        break;
    case '<':
        break;
    default:
        //Es una operación de único carácter
        devolverCaracter();
        buffer[1] = '\0';
        a = crearNodo(buffer, buffer[0]);
        return a;
    }
    return NULL;
}
