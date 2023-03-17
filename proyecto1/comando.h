#ifndef COMANDO_H
#define COMANDO_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <ctime>
//#include 

using namespace std;

//Podría añadir seguido del id la ubicacion del disco 
string ids = "";

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
    string Fs = " "; //Para el formateo completo de la partición (mkfs)
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

typedef struct{
    int s_filesystem_type;
    int s_inodes_count;
    int s_blocks_count;
    int s_free_blocks_count;
    int s_free_inodes_count;
    time_t s_mtime;
    time_t s_umtime;
    int s_mnt_count;
    int s_magic;
    int s_inode_s;
    int s_block_s;
    int s_firts_ino;
    int s_first_blo;
    int s_bm_inode_start;
    int s_bm_block_start;
    int s_inode_start;
    int s_block_start;
}SuperBloque;

typedef struct{
    int i_uid;
    int i_gid;
    int i_s;
    time_t i_atime;
    time_t c_ctime;
    time_t i_mtime;
    int i_block;
    char i_type;
    int i_perm;
}Inodos;

typedef struct{
    char b_name[12];
    int b_nodo;
}Content;

typedef struct{
    Content b_content[4];
}Carperta;

class Comando
{
    public:
        Parametros param;
        vector<string> split_id(string entrada);
        void identificarCMD(Parametros p);
        void crear_archivo(string tam, string ubicacion, string fit, string unit);
        void crear_carpeta(string ubi);
        void eliminar_archivo(string ubi);
        void crear_particion(string tam, string ubicacion, string nombre, string unit,
        string tipo, string fit, string Delete, string Add);
        void montar(string ubi, string nombre);
        void desmontar(string id);
        bool existeId(string id);
        void formatear(string id, string type, string fs);
};

#endif