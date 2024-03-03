#ifndef LEXICO_H
#define LEXICO_H
/*
Representación de un componente léxico, con su número y su lexema
*/
typedef struct ComponenteLexico{
    int valor;
    char* lexema;
    struct ComponenteLexico* izq;
    struct ComponenteLexico* der;
}ComponenteLexico;

//Función utilizada por el analizador sintactico para solicitar al analizador lexico un componente lexico
ComponenteLexico* siguienteComponenteLexico();

#endif  // LEXICO_H