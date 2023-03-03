#include "comando.h"
#include <fstream>
#include <cstdio>



void Comando::identificarCMD(Parametros param)
{
    if(param.Comando=="execute"){
        if(param.Ubicacion != " "){
            cout<<" Se llama a la función para abrir el archivo"<<endl;
        }else{
            cout << "Error al buscar la dirección del archivo." << endl;
        }
    }
}