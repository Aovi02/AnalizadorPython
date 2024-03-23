#ifndef ERRORES_H
#define ERRORES_H

//Todos los errores son bastante explicativos por su nombre
//Lo único que hacen es un printf
void errorLexemaGrande();
void errorLiteralMalformado(int linea);
void errorFloatMalformado(int linea);
void errorEnteroMalformado(int linea);
void errorLexemaNoReconocido(int linea);
void errorAperturaArchivo();

#endif //ERRORES_H