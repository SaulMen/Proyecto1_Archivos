#include "comando.h"
#include <fstream>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <random>
#include <ctime>
#include <thread>
#include <chrono>


using std::this_thread::sleep_for;
using namespace std::chrono_literals;



void Comando::identificarCMD(Parametros param)
{
    if(param.Comando=="execute"){
        if(param.Ubicacion != " "){
            cout<<" Se llama a la función para abrir el archivo"<<endl;
        }else{
            cout << "Error al buscar la dirección del archivo." << endl;
        }
    }else if(param.Comando=="mkdisk"){
        if(stoi(param.Tamano)>0 && param.Ubicacion!=" "){
            crear_carpeta(param.Ubicacion);
            crear_archivo(param.Tamano, param.Ubicacion, param.Fit, param.Dimensional);
            cout<<"Se creó el archivo binario"<<endl;
        }else{
            cout<<"Error, parametros no aceptados"<<endl;
        }
    }else if(param.Comando=="rmdisk"){
        if(param.Ubicacion!=" "){
            eliminar_archivo(param.Ubicacion);
        }else{
            cout<<"Error, parametros no acepatados"<<endl;
        }
    }else if(param.Comando=="fdisk"){
        //sleep_for(300ms);
        if(stoi(param.Tamano)>0 && param.Ubicacion!="" && param.Nombre!=""){
            if((param.Delete==" " && param.Add==" ")||(param.Delete==" " && param.Add!=" ")||
            (param.Delete!=" " && param.Add==" ")){
                crear_particion(param.Tamano, param.Ubicacion, param.Nombre, param.Unit,
                param.Tipo, param.Fit, param.Delete, param.Add);
            }else{
                cout<<"Error, parametros no acepatados aquí"<<endl;//validar que parametros fallaron
            }                                           //Pero no prioritario
        }else{
            cout<<"Error, parametros no acepatados o aquí"<<endl;
        }
    }else if(param.Comando=="mount"){
        cout<<"Se montará la partición"<<endl;
        montar(param.Ubicacion, param.Nombre);
    }else if(param.Comando=="unmount"){
        desmontar(param.Id);
        cout<<"Se desmontará la partición"<<endl;
    }else if(param.Comando=="mkfs"){
        if(existeId(param.Id)){
            formatear(param.Id, param.Tipo, param.Fs);
        }else{
            cout<<"Error, no existe el ID"<<endl;
        }
    }
}

vector<string> Comando::split_id(string entrada){
    stringstream text_to_split(entrada);
    string segment;
    vector<string> splited;

    while(std::getline(text_to_split, segment, ',')){
        splited.push_back(segment);
    }
    return splited;
}

void Comando::crear_carpeta(string ubicacion){

    string ruta = ubicacion;
    int longitud = ubicacion.size();
    while(ruta.at(longitud-1) != '/'){
        ruta.pop_back();
        longitud --;
    }
    if(ruta.find("\"") == 0){
        ruta.erase(0,1);
    }

    cout<<"la ruta sin la extension es: "<<ruta<<endl;

    FILE *file;

    file = fopen(ubicacion.c_str(), "r");
    if (file != NULL) {
        cout<<"hola jeje"<<endl;
        fclose(file);
    }else{ // si no me abrió la direccion es porque no existe el directorio y se crea
    //Si el directorio no existe crear un array de string que a la vez me splitee
    // por diagonales y asi obtener cada carpeta, pero eliminar la ultima
    //ya que esa contendrá el archivo con extension
        cout<<"la ruta del archivo no existe\nCreando carpeta..."<<endl;
        int status = mkdir(ruta.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (status == -1 and errno == EACCES) {
            cout << "Error al crear el directorio" << endl;
            cout<<"Error de acceso (acceso denegado)"<<endl;
            //exit(EXIT_FAILURE);
        }else if(status == -1 and errno == EEXIST){
            cout<<"Error de existencia (el directorio ya existe)"<<endl;
        }else if(status == -1 and errno == EINVAL){
            cout<<"Error de existencia (el directorio ya existe)"<<endl;
        }else if(status == -1 and errno == ENOENT){
            cout<<"Error de existencia (el directorio ya existe)"<<endl;
        }else if(status == 0){
            cout<<"Se creó correctamente"<<endl;
        }
    }
}


void Comando::crear_archivo(string tam, string ubicacion, string fit, string unit)
{
    char ubicacion_char[100];
    if(ubicacion.substr(ubicacion.length()-1)=="\n") ubicacion.pop_back();
    strcpy(ubicacion_char, ubicacion.c_str());
    int size_file = 0, tamano = atoi(tam.c_str());

    char dimen = ' ';
    if(unit != " "){
        char dimen = unit.at(0);
    }else{
        char dimen = 'm';
    }
    
    if (dimen == 'k' || dimen == 'K')
    {
        size_file = tamano;
    }
    else if (dimen == 'm' || dimen == 'M')
    {
        size_file = tamano * 1024;
    }

    // Preparacion Bloque
    char bloque[1024];
    for (int i = 0; i < 1024; i++)
    {
        bloque[i] = '\0';
    }

    // Escritura de Bloque en Archivo
    int limite = 0;
    FILE *archivo_binario;
    archivo_binario = fopen(ubicacion_char, "w");
    while (limite != size_file)
    {
        fwrite(&bloque, 1024, 1, archivo_binario);
        limite++;
    }
    fclose(archivo_binario);

    //añadir el mbr al inicio
    cout<<"Añadiendo MBR..."<<endl;
    MBR mbr;
    if(unit!=" ") unit = "m";
    //para numero random
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 10000);
    int num = dis(gen);

    mbr.mbr_tamano=stoi(tam);
    mbr.mbr_fecha_creacion = time(0);
    mbr.mbr_dsk_signature=num;
    mbr.dsk_fit=fit[0];
    archivo_binario = fopen(ubicacion_char,"rb+");
    fseek(archivo_binario,0,SEEK_SET);
    fwrite(&mbr,sizeof(mbr),1,archivo_binario);
    fclose(archivo_binario);

    cout<<"Tamaño del MBR::::::::::::: "<<sizeof(MBR)<<endl;

}

void Comando::eliminar_archivo(string ubi){
    FILE *file;
    file = fopen(ubi.c_str(),"r");
    if(file != NULL){
        fclose(file);
        cout<<"Borrando archivo..."<<endl;
        if(remove(ubi.c_str())==0) cout<<"Borrado existosamente"<<endl;
        else cout<<"Error"<<endl;
    }else{
        cout<<"Error al abrir el archivo ó no existe"<<endl;
    }
}

void Comando::crear_particion(string tam, string ubicacion, string nombre, string unit,
string tipo, string fit, string Delete, string Add){

    FILE *file;
    file = fopen("/home/user/disco1.dsk", "rb+");
    MBR mbr;
    Partition parti;
    parti.part_status = 1;
    parti.part_type = tipo[0];
    parti.part_fit = fit[0];
    parti.part_s = stoi(tam);
    strcpy(parti.part_name,nombre.c_str());
    if (file != NULL) {
        cout<<"Si existe el archivo binario"<<endl;
        cout<<"Se creará la partición"<<endl;
        if(tipo[0]=='p'){
            //Añadir la opcion de que si no viene que me lo añada por defecto
            //y tambien evaluar en todas las entradas si viene el salto de linea
            //y quitarlo
            fseek(file,0, SEEK_SET);
            fread(&mbr, sizeof(mbr),1,file);
            if(mbr.mbr_partition_1.part_name[0]== '\0'){
                parti.part_start = sizeof(mbr);
                mbr.mbr_partition_1=parti;
                fseek(file,0,SEEK_SET);
                fwrite(&mbr,sizeof(mbr),1,file);

                fseek(file,parti.part_start,SEEK_SET);
                fwrite(&parti,sizeof(parti),1,file);

            }else if(mbr.mbr_partition_2.part_name[0]== '\0'){
                parti.part_start = sizeof(mbr) + mbr.mbr_partition_1.part_s;
                mbr.mbr_partition_2=parti;
                fseek(file,0,SEEK_SET);
                fwrite(&mbr,sizeof(mbr),1,file);

                fseek(file,parti.part_start,SEEK_SET);
                fwrite(&parti,sizeof(parti),1,file);
            }else if(mbr.mbr_partition_3.part_name[0]== '\0'){
                parti.part_start = sizeof(mbr) + mbr.mbr_partition_2.part_s;
                parti.part_start += mbr.mbr_partition_2.part_s;
                mbr.mbr_partition_3=parti;
                fseek(file,0,SEEK_SET);
                fwrite(&mbr,sizeof(mbr),1,file);

                fseek(file,parti.part_start,SEEK_SET);
                fwrite(&parti,sizeof(parti),1,file);
            }else if(mbr.mbr_partition_4.part_name[0]== '\0'){
                parti.part_start = sizeof(mbr) + mbr.mbr_partition_1.part_s;
                parti.part_start += mbr.mbr_partition_2.part_s + mbr.mbr_partition_3.part_s;
                mbr.mbr_partition_4=parti;
                fseek(file,0,SEEK_SET);
                fwrite(&mbr,sizeof(mbr),1,file);

                fseek(file,parti.part_start,SEEK_SET);
                fwrite(&parti,sizeof(parti),1,file);
            }else{
                cout<<"Error, ya se permiten más particiones en este disco"<<endl;
            }
        }else{

        }

        fclose(file);
    }else{
        cout<<"No existe el archivo binario.............."<<endl;
    }
}

void Comando::montar(string ubi, string nombre){
    FILE *file;
    string nombre_disco;
    string nombre_part;
    string cara;
    string id;
    //Obtengo el nombre del disco
    string ruta = ubi;
    int longitud = ubi.size();
    while(ruta.at(longitud-1) != '/'){
        cara = ruta.at(longitud-1);
        nombre_disco.insert(0,cara);
        ruta.pop_back();
        longitud --;
    }
    longitud = nombre_disco.size();
    while(nombre_disco.at(longitud-1) != '.'){
        nombre_disco.pop_back();
        longitud --;
    }
    nombre_disco.pop_back();


    nombre_part=nombre.at(nombre.size()-1);
    id="44"+nombre_part+nombre_disco;

    file = fopen(ubi.c_str(), "r");
    if (file != NULL) {
        //Mostrar un menu para ver las particiones montadas o solo mostrar de una vez
        cout<<"Montando partición con nombre: "<<id<<endl;
        ids+=id+",";
        fclose(file);
    }else{
        cout<<"Error, la ubicación no existe"<<endl;
    }
    cout<<ids<<endl;
}

void Comando::desmontar(string id){
    cout<<"Desmontando..."<<endl;

}

bool Comando::existeId(string id){
    vector<string> id_spliteado = split_id(ids);
    for(int i=0;i<id_spliteado.size();i++){
        if(id==id_spliteado.at(i)){
            cout<<"Se encontró: "<<id<<endl;
            return true;
        }
        cout<<"los ids: "<<id_spliteado.at(i)<<endl;
    }
    return false;
}

void Comando::formatear(string id, string type, string fs){
    if(fs==" ") fs = "2fs";
    cout<<"Se formaterá completamente en:"<<id<<endl;
    SuperBloque super;
    super.s_filesystem_type=2; //identificacion del sistema de archivos
    super.s_inodes_count=2; //numero actual de inodos
    super.s_blocks_count=2; //numero actual de bloques
    //super.s_free_blocks_count=98; //bloques libres
    //super.s_free_inodes_count=98; //inodos libres
    super.s_mtime=time(0); //ultima fecha que fue montado
    //super.s_umtime=time(0); //cuando se desmonte
    super.s_mnt_count=1; //cuantas veces fue montado el sistema
    super.s_magic=61267;//0xEF53
    /*super.s_inode_s=0;//Tamaño del inodo
    super.s_block_s=0;//Tamaño del bloque
    super.s_firts_ino=0;//Primer inodo libre
    super.s_first_blo=0;//primero bloque libre
    super.s_bm_inode_start=0;//inicio del bitmap de inodos
    super.s_bm_block_start=0;//inicio del bitmap de bloques
    super.s_inode_start=0;//inicion de la tabla de inodos
    super.s_block_start=0;//inicio de la table de bloques*/


    /*
        Crear mi capeta en /->invertido y mi archiov user.txt en el disco a formatear
        Validar el usuario e ir actualizando mi mbr y mis particiones

        Comenzar con las validaciones hasta lo que he hecho desde el fdisk

        Comenzar a ver como realizó los reportes-> para empezar de lleno el sábado
    */


}