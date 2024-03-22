#include "entrada.h"
#include "errores.h"
#include <stdio.h>
#include <stdlib.h>

//El archivo ya está iniciado en el main
extern FILE* archivo;

//Los componentes del sistema de entrada, los dos buffers y los dos punteros
char* bloqueA;
char* bloqueB;
char* delantero;
char* inicio;

//Para saber qué bloque tiene contenido y qué bloque no se usa este "booleano"
int esBloqueA;

//Estos son flags que ayudan a gestionar los casos al margen de los buffers
int posDelantero = 0;
int posInicio = 0;

//Indicador de elementos leidos por fread para llegar al final del archivo
int posFinal = 0;

//Flag que indica si hubo desbordamiento
int leidos = 0;

//Flag para evitar quitar bloque
int noQuitar = 0;

void iniciarEntrada(){
    bloqueA = (char*)malloc(TAM_BLOQUE);
    if(bloqueA == NULL){
        printf("Error al reservar memoria para el bloqueA\n");
        exit(EXIT_FAILURE);
    }
    bloqueB = (char*)malloc(TAM_BLOQUE);
    if(bloqueB == NULL){
        printf("Error al reservar memoria para el bloqueB\n");
        exit(EXIT_FAILURE);
    }
    posFinal = fread(bloqueA, 1, TAM_BLOQUE - 1, archivo);
    bloqueA[posFinal] = EOF;
    bloqueB[posFinal] = EOF;
    esBloqueA = 1;
    inicio = bloqueA;
    delantero = bloqueA;
}

void limpiarEntrada(){
    free(bloqueA);
    bloqueA = NULL;
    free(bloqueB);
    bloqueB = NULL;
    delantero = NULL;
    inicio = NULL;
}

char* siguienteCaracter(int lexemaSigue){

    char* r = delantero;
    if(!lexemaSigue)
        leidos= 0;

    if(delantero != NULL && *delantero == EOF){
        //Si delantero ha llegao a un EOF necesito ver si es fin de bloque A, B o si se acabó como tal el archivo
        //Estamos en bloque A, hay que reservar memoria para B
        if(esBloqueA){
            if(!noQuitar){
                posFinal = fread(bloqueB, 1, TAM_BLOQUE - 1, archivo);
                bloqueB[posFinal] = EOF;
                noQuitar = 0;
            }
            //Movemos delantero a su posicion nueva
            delantero = bloqueB;
            posDelantero = TAM_BLOQUE;
            esBloqueA = 0;
            //Liberamos A solo si el lexema no sigue por aquí
            if(!lexemaSigue){
                inicio = delantero;
            }
            r = delantero;
            delantero++;
            if(leidos > TAM_BLOQUE && lexemaSigue){
                errorLexemaGrande();
                leidos = 0;
                return NULL;
            }
            leidos++;
            posDelantero++;
            return r;
        }
        //Lo mismo de antes pero para B
        else{
            if(!noQuitar){
                posFinal = fread(bloqueA, 1, TAM_BLOQUE - 1, archivo);
                bloqueA[posFinal] = EOF;
                noQuitar = 0;
            }
            //Movemos delantero a su posicion nueva
            delantero = bloqueA;
            posDelantero = 0;
            esBloqueA = 1;
            //Liberamos B bajo el mismo criterio que A
            if(!lexemaSigue){
                inicio = delantero;
            }
            r = delantero;
            delantero++;
            if(leidos > TAM_BLOQUE && lexemaSigue){
                errorLexemaGrande();
                leidos = 0;
                return NULL;
            }
            leidos++;
            posDelantero++;
            return r;
        }
    }
    

    
    //Antes de avanzar con el delantero, está bien comprobar que el lexema no se pasa del tamaño del bloque
    //El motivo de comprobarlo aquí y no más abajo cuando se suma delantero es cuestión de eficiencia
    //Si lo compruebo aquí y resulta que el lexema se pasa, puedo ir alternando entre los bloques hasta que se acabe de leer
    if(leidos > TAM_BLOQUE && lexemaSigue){
        errorLexemaGrande();
        leidos = 0;
        return NULL;
    }
    leidos++;
    delantero++;
    if(!lexemaSigue){
        inicio = delantero;
        posInicio = posDelantero;
    }
    posDelantero++;
    return r;
}

void devolverCaracter(){
    //Si el puntero de delantero está justo al principio del bloqueB
    if(posDelantero == TAM_BLOQUE){
        //Lo coloco en el EOF de bloqueA
        delantero = bloqueA + (TAM_BLOQUE - 1);
        posDelantero = TAM_BLOQUE - 1;
        noQuitar = 1;
    }
    //Si el puntero de delantero está justo al principio del bloqueA
    else if(posDelantero == 0){
        //Lo coloco en el EOF de bloqueB
        delantero = bloqueB + (TAM_BLOQUE - 1);
        posDelantero = TAM_BLOQUE*2 - 2;
        noQuitar = 1;
    }
    //Está en medio, y se puede restar bien
    else{
        delantero--;
        posDelantero--;
    }

    //Como devolver un caracter implica haber encontrado un lexema válido, puedo mover inicio junto a delantero
    //Si inicio estaba en un bloque distinto al de delantero, puedo borrarlo aquí
    //Caso en el que inicio está en A y el lexema acaba en B, puedo borrar A
    if(posInicio < TAM_BLOQUE && posDelantero >= TAM_BLOQUE){
        esBloqueA = 0;
    }
    //El caso complementario
    else if(posInicio >= TAM_BLOQUE && posDelantero < TAM_BLOQUE){
        esBloqueA = 1;
    }
    leidos = 0;
    inicio = delantero;
    posInicio = posDelantero;
}
