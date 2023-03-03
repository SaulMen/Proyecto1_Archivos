#ifndef COMANDO_H
#define COMANDO_H

#include <iostream>

using namespace std;

typedef struct{
    string Comando = " ";
    string Tamano = " ";
    string Dimensional = " ";
    string Ubicacion = " ";
    string X = " ";
}Parametros;

class Comando
{
    public:
        Parametros param;
        void identificarCMD(Parametros p);
};

#endif