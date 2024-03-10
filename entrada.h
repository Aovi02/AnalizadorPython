#ifndef ENTRADA_H
#define ENTRADA_H

#define TAM_BLOQUE 15

//Devuelve un puntero al caracter actual
//El sistema de entrada no va a saber si el lexema se ha acabado o no, por lo que se puede dar el caso de un lexema dividido en 2 bloques.
//Para evitar perder el inicio, el lexico le va a decir al sistema de entrada si el lexema sigue o no
//Si sigue, no se borra el bloque actual hasta que se devuelva caracter
char* siguienteCaracter(int lexemaSigue);

void iniciarEntrada();

//Indica al sistema de entrada que se devuelve un caracter, marca el final de un lexema
void devolverCaracter();

#endif //ENTRADA_H