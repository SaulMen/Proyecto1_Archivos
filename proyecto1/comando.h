#ifndef COMANDO_H
#define COMANDO_H

#include <iostream>
//#include 

using namespace std;

typedef struct{
    string Comando = " ";
    string Tamano = " ";
    string Dimensional = " ";
    string Ubicacion = " ";
    string Fit = " ";
    string Unit = " ";
    string Nombre = " ";
    string Tipo = " ";
    string Delete = " ";
    string Add = " ";
    string Id = " "; //para el mount y tal vez para más
}Parametros;

typedef struct{
    char part_status;//1->activa 0->No activa
    char part_type;
    char part_fit;
    int part_start;
    int part_s;
    char part_name[16];
}Partition;

typedef struct{
    char part_status;
    char part_fit;
    char part_start;
    char part_s;
    char part_next = -1;
    char part_name[16];
}EBR;

typedef struct{
    int mbr_tamano;
    time_t mbr_fecha_creacion;
    int mbr_dsk_signature;
    char dsk_fit;
    Partition mbr_partition_1;
    Partition mbr_partition_2;
    Partition mbr_partition_3;
    Partition mbr_partition_4;
}MBR;

class Comando
{
    public:
        Parametros param;
        void identificarCMD(Parametros p);
        void crear_archivo(string tam, string ubicacion, string fit, string unit);
        void crear_carpeta(string ubi);
        void eliminar_archivo(string ubi);
        void crear_particion(string tam, string ubicacion, string nombre, string unit,
        string tipo, string fit, string Delete, string Add);
};

#endif