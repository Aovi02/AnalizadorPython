#include "entrada.h"
#include "errores.h"
#include <stdio.h>
#include <stdlib.h>

//El archivo ya está iniciado en el main, por eso usamos extern
extern FILE* archivo;

//Los componentes del sistema de entrada, los dos buffers y los dos punteros necesarios
char* bloqueA;
char* bloqueB;
char* delantero;
char* inicio;

//Para saber qué bloque se está usando actualmente
int esBloqueA;

//Estos son flags que ayudan a gestionar los casos margen
//Concretamente, ayudan a gestionar los casos en los que se devuelve un caracter y hay que marcar que no se debe quitar el buffer
int posDelantero = 0;
int posInicio = 0;

//Indicador de elementos leidos por fread para llegar al final del archivo, ayuda a colocar el EOF al final del buffer
int posFinal = 0;

//Contador de caracteres leidos, utilizado para indicar desbordamietnto
int leidos = 0;

//Flag para evitar quitar bloque cuando éste se sigue usando pero se ha devuelto un caracter
int noQuitar = 0;

void iniciarEntrada(){
    //Iniciamos ambos bloques
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
    //Leemos contenido para el primer bloque
    posFinal = fread(bloqueA, 1, TAM_BLOQUE - 1, archivo);
    bloqueA[posFinal] = EOF;
    bloqueB[posFinal] = EOF;
    //Marcamos el bloque usado y ajustamos los punteros correctamente
    esBloqueA = 1;
    inicio = bloqueA;
    delantero = bloqueA;
}

void limpiarEntrada(){
    //Liberamos los bloques y colocamos todos los punteros a NULL
    free(bloqueA);
    bloqueA = NULL;
    free(bloqueB);
    bloqueB = NULL;
    delantero = NULL;
    inicio = NULL;
}

char* siguienteCaracter(int lexemaSigue){

    //Guardamos el caracter actual
    char* r = delantero;
    if(!lexemaSigue)
        leidos= 0;

    if(delantero != NULL && *delantero == EOF){
        //Si delantero ha llegado a un EOF necesito ver si es fin de bloque A, B o si se acabó como tal el archivo
        //Estamos en bloque A, hay que leer contenido en el bloque B
        if(esBloqueA){
            //Se lee contenido para el bloque B sólo si noQuitar está a 0 (es decir, que se puede quitar bloque)
            if(!noQuitar){
                posFinal = fread(bloqueB, 1, TAM_BLOQUE - 1, archivo);
                bloqueB[posFinal] = EOF;
                noQuitar = 0;
            }
            //Movemos delantero a su posicion nueva y actualizamos variables
            delantero = bloqueB;
            posDelantero = TAM_BLOQUE;
            esBloqueA = 0;
            //Optimización, si estamos en un comentario, el inicio también se mueve
            if(!lexemaSigue){
                inicio = delantero;
                posInicio = posDelantero;
            }
            //Leemos un nuevo caracter para no pasarle EOF al sistema de entrada
            r = delantero;
            delantero++;
            //Hay que comprobar de antemano si el lexema se pasa, porque el lexico siempre comprobará un caracter de más antes de saber si tiene que parar
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
            if(!lexemaSigue){
                inicio = delantero;
                posInicio = posDelantero;
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
    
    //Si no se lee el EOF tengo que hacer la comprobación aquí también
    if(leidos > TAM_BLOQUE && lexemaSigue){
        errorLexemaGrande();
        leidos = 0;
        return NULL;
    }
    leidos++;
    delantero++;
    //Este cacho es la misma actualización para mover incio junto a delantero
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
        //Lo coloco en el EOF de bloqueA e indico que no se debe quitar el bloque, porque en la siguiente lectura nos encontraremos con un EOF
        delantero = bloqueA + (TAM_BLOQUE - 1);
        posDelantero = TAM_BLOQUE - 1;
        noQuitar = 1;
    }
    //Si el puntero de delantero está justo al principio del bloqueA
    else if(posDelantero == 0){
        //Lo mismo de antes
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
    if(posInicio < TAM_BLOQUE && posDelantero >= TAM_BLOQUE){
        esBloqueA = 0;
    }
    //El caso complementario
    else if(posInicio >= TAM_BLOQUE && posDelantero < TAM_BLOQUE){
        esBloqueA = 1;
    }
    //Por lo explicado hace 2 comentarios también reseteo el contador de caracteres leidos
    leidos = 0;
    inicio = delantero;
    posInicio = posDelantero;
}
