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
        }else{
            cout<<"Error, parametros no aceptados"<<endl<<endl;
        }
    }else if(param.Comando=="rmdisk"){
        if(param.Ubicacion!=" "){
            eliminar_archivo(param.Ubicacion);
        }else{
            cout<<"Error, parametro 'path' no acepatados"<<endl;
        }
    }else if(param.Comando=="fdisk"){
        if(param.Delete==" "&&param.Add==" "){
            if(param.Tamano!=" "){
                if(stoi(param.Tamano)>0 && param.Ubicacion!="" && param.Nombre!=""){
                    crear_particion(param.Tamano, param.Ubicacion, param.Nombre, param.Unit,
                        param.Tipo, param.Fit, param.Delete, param.Add);
                }else{
                    cout<<"Error, parametros no acepatados o aquí"<<endl<<endl;
                }
            }else {
                cout<<"Error, no se reconoció parametro size"<<endl<<endl;
            }
        }else if((param.Delete==" "&&param.Add!=" ")||(param.Delete!=" " && param.Add==" ")){
            if(param.Delete!=" "){
                cout<<"Se va a eliminar"<<endl;
                eliminar_particion(param.Nombre, param.Ubicacion);
            }else{
                if(param.Add!=" "){
                    cout<<"Se va a agregar"<<endl;
                    agregar_particion(param.Add,param.Unit,param.Nombre,param.Ubicacion);
                }else{
                    cout<<"Error, no se reconoció parametro add"<<endl<<endl;
                }
            }
        }else{
            cout<<"Error, parametros 'add' y 'delete' al mismo tiempo"<<endl<<endl;
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
    }else if(param.Comando=="rep"){
        reporte(param.Id, param.Ubicacion, param.Nombre);
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

    //cout<<"la ruta sin la extension es: "<<ruta<<endl;

    FILE *file;

    file = fopen(ubicacion.c_str(), "r");
    if (file != NULL) {
        fclose(file);
    }else{ // si no me abrió la direccion es porque no existe el directorio y se crea
    //Si el directorio no existe crear un array de string que a la vez me splitee
    // por diagonales y asi obtener cada carpeta, pero eliminar la ultima
    //ya que esa contendrá el archivo con extension
        
        int status = mkdir(ruta.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (status == -1 and errno == EACCES) {
            cout << "Error al crear el directorio" << endl;
            cout<<"Error de acceso (acceso denegado)"<<endl;
            //exit(EXIT_FAILURE);
        }else if(status == -1 and errno == EEXIST){
            cout<<"Error de existencia (el directorio/carpeta ya existe)"<<endl;
        }else if(status == -1 and errno == EINVAL){
            cout<<"Error de existencia (el directorio/carpeta ya existe)"<<endl;
        }else if(status == -1 and errno == ENOENT){
            cout<<"Error de existencia (el directorio/carpeta ya existe)"<<endl;
        }else if(status == 0){
            cout<<"la ruta del archivo no existe, creando carpeta..."<<endl;
        }
    }
}


void Comando::crear_archivo(string tam, string ubicacion, string fit, string unit)
{
    char ubicacion_char[100];
    if(ubicacion.substr(ubicacion.length()-1)=="\n") ubicacion.pop_back();
    strcpy(ubicacion_char, ubicacion.c_str());
    int size_file = 0, tamano = atoi(tam.c_str());

    char dimen;
    if(unit!= " "){
        dimen = unit[0];
    }else{
        dimen = 'm';
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

    if(fopen(ubicacion_char,"r")!=NULL){
        cout<<"El archivo binario ya existe"<<endl;
        return;
    }

    archivo_binario = fopen(ubicacion_char, "w");
    while (limite != size_file)
    {
        fwrite(&bloque, 1024, 1, archivo_binario);
        limite++;
    }
    fclose(archivo_binario);

    /*cout << "   (Enter para continuar)";
    char cadena;
    bool check = true;
    while(cin.get(cadena) && check)
    {
        if( check = (cadena != '\n') )
        {
            cout << cadena << '\n';
        }
    }*/

    //añadir el mbr al inicio
    cout<<"Añadiendo MBR..."<<endl;
    MBR mbr;
    if(unit[0]==' ') unit = "m";
    //para numero random
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 10000);
    int num = dis(gen);

    mbr.mbr_tamano=size_file*1024;
    mbr.mbr_fecha_creacion = time(0);
    mbr.mbr_dsk_signature=num;
    mbr.dsk_fit=fit[0];
    archivo_binario = fopen(ubicacion_char,"rb+");
    fseek(archivo_binario,0,SEEK_SET);
    fwrite(&mbr,sizeof(mbr),1,archivo_binario);
    fclose(archivo_binario);

    //cout<<"Tamaño del MBR::::::::::::: "<<sizeof(MBR)<<endl;
    cout<<"Se creó el archivo binario"<<endl<<endl;
}

void Comando::eliminar_archivo(string ubi){
    FILE *file;
    file = fopen(ubi.c_str(),"r");
    char des=' ';
    if(file != NULL){
        fclose(file);
        cout<<"Seguro que desea eliminar el archivo? (s/n):";
        cin>>des;
        if(des=='s'||des=='S'){
            cout<<"Borrando archivo..."<<endl;
            if(remove(ubi.c_str())==0) cout<<"Borrado existosamente"<<endl;
            else cout<<"Error al borrar el archivo"<<endl;
        }else{

        }
    }else{
        cout<<"Error, el archivo no existe"<<endl;
    }
}

void Comando::crear_particion(string tam, string ubicacion, string nombre, string unit,
string tipo, string fit, string Delete, string Add){

    int tamano_total;
    if(unit==" "){
        unit = "k";
        tamano_total = 1024*stoi(tam);
    }else if(unit=="b"||unit=="B"){
        tamano_total = stoi(tam);
    }else if(unit=="k"||unit=="K"){
    tamano_total = 1024*stoi(tam);
    }else if(unit=="m"||unit=="M"){
        tamano_total = 1024*1024*stoi(tam);
    }

    FILE *file;
    file = fopen(ubicacion.c_str(), "rb+");
    MBR mbr;
    Partition parti;
    EBR ebr;
    parti.part_status = 0;
    parti.part_type = tipo[0];
    parti.part_fit = fit[0];
    parti.part_s = tamano_total;
    strcpy(parti.part_name,nombre.c_str());
    if (file != NULL) {
        cout<<"Archivo binario encontrado, creando partición: "<<endl;
        if(tipo[0]==' ') tipo="p";
        if(tipo[0]=='p'){
            cout<<"Se creará una partición primaria"<<endl<<endl;
            //Me ubico en el inicio de la particion 1
            /*fseek(file, (sizeof(mbr) - (4*sizeof(parti))) , SEEK_END);
            fread(&mbr, sizeof(mbr),1,file);
            cout<<"nombre particion 1: "<<parti.part_name<<endl;*/

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
                cout<<"Error, ya se no permiten más particiones en este disco"<<endl;
            }
        }else if(tipo[0]=='e'){
            cout<<"Se creará una partición extendida"<<endl<<endl;
            fseek(file,0, SEEK_SET);
            fread(&mbr, sizeof(mbr),1,file);
            if(mbr.mbr_partition_1.part_name[0]== '\0'){
                parti.part_start = sizeof(mbr);
                mbr.mbr_partition_1=parti;
                fseek(file,parti.part_start,SEEK_SET);
                ebr.part_fit=fit[0];
                ebr.part_status='0';
                ebr.part_start=sizeof(mbr);
                ebr.part_s=tamano_total;
                ebr.part_next=-1;
                strcpy(ebr.part_name,nombre.c_str());
                fwrite(&ebr,sizeof(ebr),1,file);

            }else if(mbr.mbr_partition_2.part_name[0]== '\0'){
                if(mbr.mbr_partition_1.part_type=='e'){
                    cout<<"Error, ya existe una partición extendida" <<endl;
                    return;
                }else{
                    parti.part_start = sizeof(mbr);
                    if(mbr.mbr_partition_1.part_name[0]!= '\0'){
                        parti.part_start += mbr.mbr_partition_1.part_s;
                    }
                    mbr.mbr_partition_2=parti;
                    fseek(file,parti.part_start,SEEK_SET);
                    ebr.part_fit=fit[0];
                    ebr.part_status='0';
                    ebr.part_start=sizeof(mbr);
                    ebr.part_s=tamano_total;
                    ebr.part_next=-1;
                    strcpy(ebr.part_name,nombre.c_str());
                    fwrite(&ebr,sizeof(ebr),1,file);
                    }
            }else if(mbr.mbr_partition_3.part_name[0]== '\0'){
                if(mbr.mbr_partition_1.part_type=='e'||mbr.mbr_partition_2.part_type=='e'){
                    cout<<"Error, ya existe una partición extendida" <<endl;
                    return;
                }else{
                    parti.part_start = sizeof(mbr)+mbr.mbr_partition_1.part_s+mbr.mbr_partition_2.part_s;
                    mbr.mbr_partition_3=parti;
                    fseek(file,parti.part_start,SEEK_SET);
                    ebr.part_fit=fit[0];
                    ebr.part_status='0';
                    ebr.part_start=sizeof(mbr);
                    ebr.part_s=tamano_total;
                    ebr.part_next=-1;
                    strcpy(ebr.part_name,nombre.c_str());
                    fwrite(&ebr,sizeof(ebr),1,file);
                }
            }else if(mbr.mbr_partition_4.part_name[0]== '\0'){
                if(mbr.mbr_partition_1.part_type=='e'||mbr.mbr_partition_2.part_type=='e'||
                mbr.mbr_partition_3.part_type=='e'){
                    cout<<"Error, ya existe una partición extendida" <<endl;
                    return;
                }else{
                    parti.part_start = sizeof(mbr);
                    parti.part_start+=mbr.mbr_partition_1.part_s+mbr.mbr_partition_2.part_s;
                    parti.part_start+=mbr.mbr_partition_3.part_s;
                    mbr.mbr_partition_4=parti;
                    fseek(file,parti.part_start,SEEK_SET);
                    ebr.part_fit=fit[0];
                    ebr.part_status='0';
                    ebr.part_start=sizeof(mbr);
                    ebr.part_s=tamano_total;
                    ebr.part_next=-1;
                    strcpy(ebr.part_name,nombre.c_str());
                    fwrite(&ebr,sizeof(ebr),1,file);
                }
            }else{
                cout<<"Error, ya se no permiten más particiones en este disco"<<endl;
            }

        }else if(tipo[0]=='l'){
            EBR ebr2;
            cout<<"Se creará una partición lógica"<<endl<<endl;
            if(mbr.mbr_partition_1.part_type=='e'){
                fseek(file,mbr.mbr_partition_1.part_s,SEEK_SET);
                fread(&ebr, sizeof(ebr),1,file);
                while(ebr.part_next!=-1){
                    fseek(file,ebr.part_next,SEEK_SET);
                    fread(&ebr, sizeof(ebr),1,file);
                }
                ebr2.part_fit=fit[0];
                ebr2.part_status='0';
                ebr2.part_start=ebr.part_s;
                ebr2.part_s=tamano_total;
                ebr2.part_next=ebr.part_s+tamano_total;
                fseek(file,ebr.part_s,SEEK_SET);
                fwrite(&ebr,sizeof(ebr),1,file);
            }else if(mbr.mbr_partition_2.part_type=='e'){
                fseek(file,mbr.mbr_partition_2.part_s,SEEK_SET);
                fread(&ebr, sizeof(ebr),1,file);
                if(ebr.part_next==-1){
                    
                }
                ebr2.part_fit=fit[0];
                ebr2.part_status='0';
                ebr2.part_start=ebr.part_s;
                ebr2.part_s=tamano_total;
                ebr2.part_next=ebr.part_s+tamano_total;
                fseek(file,ebr2.part_start,SEEK_SET);
                fwrite(&ebr2,sizeof(ebr2),1,file);
            }else if(mbr.mbr_partition_3.part_type=='e'){
                fseek(file,mbr.mbr_partition_3.part_s,SEEK_SET);
                fread(&ebr, sizeof(ebr),1,file);
                if(ebr.part_next==-1){
                    
                }
                ebr2.part_fit=fit[0];
                ebr2.part_status='0';
                ebr2.part_start=ebr.part_s;
                ebr2.part_s=tamano_total;
                ebr2.part_next=ebr.part_s+tamano_total;
                fseek(file,ebr2.part_start,SEEK_SET);
                fwrite(&ebr2,sizeof(ebr2),1,file);
            }else if(mbr.mbr_partition_4.part_type=='e'){
                fseek(file,mbr.mbr_partition_4.part_s,SEEK_SET);
                fread(&ebr, sizeof(ebr),1,file);
                if(ebr.part_next==-1){
                    
                }
                ebr2.part_fit=fit[0];
                ebr2.part_status='0';
                ebr2.part_start=ebr.part_s;
                ebr2.part_s=tamano_total;
                ebr2.part_next=ebr.part_s+tamano_total;
                fseek(file,ebr2.part_start,SEEK_SET);
                fwrite(&ebr2,sizeof(ebr2),1,file);
            }else{
                cout<<"Error, no existe ninguna partición extendida"<<endl;
            }
        }else{
            cout<<"Error, parametro para partición no aceptado:"<<tipo[0]<<endl;
        }

        fclose(file);
    }else{
        cout<<"No existe el archivo binario.............."<<endl;
    }
}

void Comando::eliminar_particion(string nombre, string ubicacion){
    FILE *file;
    MBR mbr;
    Partition parti;
    file = fopen(ubicacion.c_str(), "rb+");
    if (file != NULL) {
        cout<<"Archivo binario encontrado"<<endl;
        fseek(file,0, SEEK_SET);
        fread(&mbr, sizeof(mbr),1,file);
        if(mbr.mbr_partition_1.part_name==nombre){
            cout<<"Se econtró la partición: "<<nombre<<endl;
            
            char bloque='\0';
            int inicio = mbr.mbr_partition_1.part_start;//inicio de la particion
            int limite = 0;
            fseek(file,inicio,SEEK_SET);
            while (limite != sizeof(mbr.mbr_partition_1))
            {
                fwrite(&bloque, 1, 1, file);
                limite++;
            }
            cout<<"Se eliminó la partición: "<<nombre<<" correctamente"<<endl;
            mbr.mbr_partition_1=parti;
            fclose(file);

        }else if(mbr.mbr_partition_2.part_name==nombre){
            cout<<"Se econtró la partición: "<<nombre<<endl;
            
            char bloque='\0';
            int inicio = mbr.mbr_partition_2.part_start;//inicio de la particion
            int limite = 0;
            fseek(file,inicio,SEEK_SET);
            while (limite != sizeof(mbr.mbr_partition_2))
            {
                fwrite(&bloque, 1, 1, file);
                limite++;
            }
            cout<<"Se eliminó la partición: "<<nombre<<" correctamente"<<endl;
            mbr.mbr_partition_2=parti;
            fclose(file);

        }else if(mbr.mbr_partition_3.part_name==nombre){
            cout<<"Se econtró la partición: "<<nombre<<endl;
            
            char bloque='\0';
            int inicio = mbr.mbr_partition_3.part_start;//inicio de la particion
            int limite = 0;
            fseek(file,inicio,SEEK_SET);
            while (limite != sizeof(mbr.mbr_partition_3))
            {
                fwrite(&bloque, 1, 1, file);
                limite++;
            }
            cout<<"Se eliminó la partición: "<<nombre<<" correctamente"<<endl;
            mbr.mbr_partition_3=parti;
            fclose(file);

        }else if(mbr.mbr_partition_4.part_name==nombre){
            cout<<"Se econtró la partición: "<<nombre<<endl;
            
            char bloque='\0';
            int inicio = mbr.mbr_partition_4.part_start;//inicio de la particion
            int limite = 0;
            fseek(file,inicio,SEEK_SET);
            while (limite != sizeof(mbr.mbr_partition_4))
            {
                fwrite(&bloque, 1, 1, file);
                limite++;
            }
            cout<<"Se eliminó la partición: "<<nombre<<" correctamente"<<endl;
            mbr.mbr_partition_4=parti;
            fclose(file);
        }else{
            cout<<"Error, no se encontro la particion con nombre:"<<nombre<<endl;
        }

    }else{
        cout<<"Error, no se encontró el archivo"<<endl;
    }

}

void Comando::agregar_particion(string tam, string unit, string nombre, string ubicacion){
    FILE *file;
    MBR mbr;
    Partition parti;
    file = fopen(ubicacion.c_str(), "rb+");
    char nulo='\0';
    char bloque[1024];
    for (int i = 0; i < 1024; i++)
    {
        bloque[i] = '\0';
    }
    if(unit==" ") unit ="b";
    if (file != NULL){
        cout<<"Archivo binario encontrado"<<endl;
        if(stoi(tam)>=0){
            //Se añadirá
            fseek(file,0, SEEK_SET);
            fread(&mbr, sizeof(mbr),1,file);
            if(mbr.mbr_partition_1.part_name==nombre){
                return;
                cout<<"Se econtró la partición: "<<nombre<<endl;
                if(mbr.mbr_partition_2.part_name!=""&&mbr.mbr_partition_3.part_name!=""&&
                mbr.mbr_partition_4.part_name!=""){
                    //hay espacio
                    int espacio_restante=mbr.mbr_tamano-mbr.mbr_partition_1.part_s;
                    int inicio = mbr.mbr_partition_1.part_s+mbr.mbr_partition_1.part_start;
                    int limite = 0;
                    if(unit=="k"){
                        int tam_nuevo=stoi(tam)*1024;
                        if(tam_nuevo<=espacio_restante){

                            fseek(file,inicio, SEEK_SET);
                            while (limite != tam_nuevo)
                            {
                                fwrite(&bloque, 1024, 1, file);
                                limite++;
                            }
                            fclose(file);
                            cout<<"Se añadido a la particion: "<<tam<<"k"<<endl;
                            mbr.mbr_partition_1.part_s+=tam_nuevo;
                        }else{
                            cout<<"No hay espacio suficiente en el disco"<<endl;
                        }
                    }else if(unit=="m"){
                        limite = 0;
                        int tam_nuevo=stoi(tam)*1024*1024;
                        if(tam_nuevo<=espacio_restante){
                            fseek(file,inicio, SEEK_SET);
                            while (limite != tam_nuevo)
                            {
                                fwrite(&bloque, 1024, 1, file);
                                limite++;
                            }
                            fclose(file);
                            cout<<"Se añadido a la particion: "<<tam<<"m"<<endl;
                            mbr.mbr_partition_1.part_s+=tam_nuevo;
                        }else{
                            cout<<"No hay espacio suficiente en el disco"<<endl;
                        }
                    }else{
                        //es byte
                        limite = 0;
                        if(stoi(tam)<=espacio_restante){
                            fseek(file,inicio, SEEK_SET);
                            while (limite != stoi(tam))
                            {
                                fwrite(&nulo, 1, 1, file);
                                limite++;
                            }
                            fclose(file);
                            cout<<"Se añadido a la particion: "<<tam<<"m"<<endl;
                            mbr.mbr_partition_1.part_s+=stoi(tam);
                        }else{
                            cout<<"No hay espacio suficiente en el disco"<<endl;
                        }
                    }
                }else if(mbr.mbr_partition_2.part_name==""){
                    cout<<"Error, no es posible añadir espacio (particion adelante)"<<endl;
                }else{
                    return;
                }

            }else if(mbr.mbr_partition_2.part_name==nombre){
                cout<<"Se econtró la partición: "<<nombre<<endl;
                return;
                if(mbr.mbr_partition_3.part_name!=""&&mbr.mbr_partition_4.part_name!=""){
                    //hay espacio
                    int espacio_restante=mbr.mbr_tamano-mbr.mbr_partition_2.part_s;
                    int inicio = mbr.mbr_partition_2.part_s+mbr.mbr_partition_2.part_start;
                    int limite = 0;
                    if(unit=="k"){
                        int tam_nuevo=stoi(tam)*1024;
                        if(tam_nuevo<=espacio_restante){

                            fseek(file,inicio, SEEK_SET);
                            while (limite != tam_nuevo)
                            {
                                fwrite(&bloque, 1024, 1, file);
                                limite++;
                            }
                            fclose(file);
                            cout<<"Se añadido a la particion: "<<tam<<"k"<<endl;
                            mbr.mbr_partition_2.part_s+=tam_nuevo;
                        }else{
                            cout<<"No hay espacio suficiente en el disco"<<endl;
                        }
                    }else if(unit=="m"){
                        limite = 0;
                        int tam_nuevo=stoi(tam)*1024;
                        if(tam_nuevo<=espacio_restante){
                            fseek(file,inicio, SEEK_SET);
                            while (limite != tam_nuevo)
                            {
                                fwrite(&bloque, 1024, 1, file);
                                limite++;
                            }
                            fclose(file);
                            cout<<"Se añadido a la particion: "<<tam<<"m"<<endl;
                            mbr.mbr_partition_2.part_s+=tam_nuevo;
                        }else{
                            cout<<"No hay espacio suficiente en el disco"<<endl;
                        }
                    }else{
                        //es byte
                        limite = 0;
                        if(stoi(tam)<=espacio_restante){
                            fseek(file,inicio, SEEK_SET);
                            while (limite != stoi(tam))
                            {
                                fwrite(&nulo, 1, 1, file);
                                limite++;
                            }
                            fclose(file);
                            cout<<"Se añadido a la particion: "<<tam<<"m"<<endl;
                            mbr.mbr_partition_2.part_s+=stoi(tam);
                        }else{
                            cout<<"No hay espacio suficiente en el disco"<<endl;
                        }
                    }
                }else if(mbr.mbr_partition_3.part_name==""){
                    cout<<"Error, no es posible añadir espacio (particion adelante)"<<endl;
                }else{return;}
            }else if(mbr.mbr_partition_3.part_name==nombre){
                cout<<"Se econtró la partición: "<<nombre<<endl;
                return;
                if(mbr.mbr_partition_4.part_name!=""){
                    //hay espacio
                    int espacio_restante=mbr.mbr_tamano-mbr.mbr_partition_3.part_s;
                    int inicio = mbr.mbr_partition_3.part_s+mbr.mbr_partition_3.part_start;
                    int limite = 0;
                    if(unit=="k"){
                        int tam_nuevo=stoi(tam)*1024;
                        if(tam_nuevo<=espacio_restante){

                            fseek(file,inicio, SEEK_SET);
                            while (limite != tam_nuevo)
                            {
                                fwrite(&bloque, 1024, 1, file);
                                limite++;
                            }
                            fclose(file);
                            cout<<"Se añadido a la particion: "<<tam<<"k"<<endl;
                            mbr.mbr_partition_3.part_s+=tam_nuevo;
                        }else{
                            cout<<"No hay espacio suficiente en el disco"<<endl;
                        }
                    }else if(unit=="m"){
                        limite = 0;
                        int tam_nuevo=stoi(tam)*1024*1024;
                        if(tam_nuevo<=espacio_restante){
                            fseek(file,inicio, SEEK_SET);
                            while (limite != tam_nuevo)
                            {
                                fwrite(&bloque, 1024, 1, file);
                                limite++;
                            }
                            fclose(file);
                            cout<<"Se añadido a la particion: "<<tam<<"m"<<endl;
                            mbr.mbr_partition_3.part_s+=tam_nuevo;
                        }else{
                            cout<<"No hay espacio suficiente en el disco"<<endl;
                        }
                    }else{
                        //es byte
                        limite = 0;
                        if(stoi(tam)<=espacio_restante){
                            fseek(file,inicio, SEEK_SET);
                            while (limite != stoi(tam))
                            {
                                fwrite(&nulo, 1, 1, file);
                                limite++;
                            }
                            fclose(file);
                            cout<<"Se añadido a la particion: "<<tam<<"m"<<endl;
                            mbr.mbr_partition_3.part_s+=stoi(tam);
                        }else{
                            cout<<"No hay espacio suficiente en el disco"<<endl;
                        }
                    }
                }else if(mbr.mbr_partition_4.part_name==""){
                    cout<<"Error, no es posible añadir espacio (particion adelante)"<<endl;
                }else{return;}
            }else if(mbr.mbr_partition_4.part_name==nombre){
                int espacio_restante=mbr.mbr_tamano-mbr.mbr_partition_3.part_s;
                int inicio = mbr.mbr_partition_3.part_s+mbr.mbr_partition_3.part_start;
                int limite = 0;
                if(unit=="k"){
                    int tam_nuevo=stoi(tam);
                    if(tam_nuevo<=espacio_restante){

                        fseek(file,inicio, SEEK_SET);
                        while (limite != tam_nuevo)
                        {
                            fwrite(&bloque, 1024, 1, file);
                            limite++;
                        }
                        fclose(file);
                        cout<<"Se añadido a la particion: "<<tam<<"k"<<endl;
                        mbr.mbr_partition_3.part_s+=tam_nuevo;
                    }else{
                        cout<<"No hay espacio suficiente en el disco"<<endl;
                    }
                }else if(unit=="m"){
                    limite = 0;
                    int tam_nuevo=stoi(tam)*1024;
                    if(tam_nuevo<=espacio_restante){
                        fseek(file,inicio, SEEK_SET);
                        while (limite != tam_nuevo)
                        {
                            fwrite(&bloque, 1024, 1, file);
                            limite++;
                        }
                        fclose(file);
                        cout<<"Se añadido a la particion: "<<tam<<"m"<<endl;
                        mbr.mbr_partition_3.part_s+=tam_nuevo;
                    }else{
                        cout<<"No hay espacio suficiente en el disco"<<endl;
                    }
                }else{
                    //es byte
                    limite = 0;
                    if(stoi(tam)<=espacio_restante){
                        fseek(file,inicio, SEEK_SET);
                        while (limite != stoi(tam))
                        {
                            fwrite(&nulo, 1, 1, file);
                            limite++;
                        }
                        fclose(file);
                        cout<<"Se añadido a la particion: "<<tam<<"m"<<endl;
                        mbr.mbr_partition_3.part_s+=stoi(tam);
                    }else{
                        cout<<"No hay espacio suficiente en el disco"<<endl;
                    }
                }
            }else{
                cout<<"Error, no se enncontro la particion con nombre:"<<nombre<<endl;
            }
        }else{
            //Se restará
            int tam_restar=0;
            if(tam=="k"||tam=="K"){
                tam_restar=stoi(tam)*1024;
            }else if(tam=="m"||tam=="M"){
                tam_restar=stoi(tam)*1024*1024;
            }else{
                tam_restar=stoi(tam);
            }

            fseek(file,0, SEEK_SET);
            fread(&mbr, sizeof(mbr),1,file);
            if(mbr.mbr_partition_1.part_name==nombre){
                cout<<"Se econtró la partición: "<<nombre<<endl;
                int disponible=mbr.mbr_partition_1.part_s - (sizeof(mbr)+sizeof(parti));
                if(disponible>tam_restar){
                    cout<<"Hay espacio para restar"<<endl;
                    mbr.mbr_partition_1.part_s-=tam_restar;
                    cout<<"Se eliminó:"<<tam_restar<<"B de la partición"<<endl;
                }else{
                    cout<<"Error, no se puede restar porque es mayor a la partición"<<endl;
                }

            }else if(mbr.mbr_partition_2.part_name==nombre){
                cout<<"Se econtró la partición: "<<nombre<<endl;
                int disponible=mbr.mbr_partition_2.part_s-(mbr.mbr_partition_2.part_start+sizeof(parti)-1);
                if(disponible>tam_restar){
                    cout<<"Hay espacio para restar"<<endl;
                    mbr.mbr_partition_1.part_s-=tam_restar;
                    cout<<"Se eliminó:"<<tam_restar<<"B de la partición"<<endl;
                }else{
                    cout<<"Error, no se puede restar porque es mayor a la partición"<<endl;
                }

            }else if(mbr.mbr_partition_3.part_name==nombre){
                cout<<"Se econtró la partición: "<<nombre<<endl;
                int disponible=mbr.mbr_partition_3.part_s-(mbr.mbr_partition_3.part_start+sizeof(parti)-1);
                if(disponible>tam_restar){
                    cout<<"Hay espacio para restar"<<endl;
                    mbr.mbr_partition_1.part_s-=tam_restar;
                    cout<<"Se eliminó:"<<tam_restar<<"B de la partición"<<endl;
                }else{
                    cout<<"Error, no se puede restar porque es mayor a la partición"<<endl;
                }

            }else if(mbr.mbr_partition_4.part_name==nombre){
                cout<<"Se econtró la partición: "<<nombre<<endl;
                int disponible=mbr.mbr_partition_4.part_s-(mbr.mbr_partition_4.part_start+sizeof(parti)-1);
                if(disponible>tam_restar){
                    cout<<"Hay espacio para restar"<<endl;
                    mbr.mbr_partition_1.part_s-=tam_restar;
                    cout<<"Se eliminó:"<<tam_restar<<"B de la partición"<<endl;
                }else{
                    cout<<"Error, no se puede restar porque es mayor a la partición"<<endl;
                }
            }else{
                cout<<"Error, no se enncontro la particion con nombre:"<<nombre<<endl;
            }
        }
    }
    else{
        cout<<"Error, no se encontró el archivo"<<endl;
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
        cout<<"Ruta de disco: "<<ubi<<endl;
        ids+=id+","+ubi+",";//El id seguido se ru ruta de disco
        imp_ids+=id+" ";

        fclose(file);

        //COMENZAR A PULIR LAS COSAS QUE TENGO
        //Y HACER LA CREACION DE CARPETAS Y ARCHIVOS EN DISCOS
    }else{
        cout<<"Error, la ubicación no existe"<<endl;
    }
    cout<<"Particiones montadas (id's): "<<imp_ids<<endl;
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
        //cout<<"los ids: "<<id_spliteado.at(i)<<endl;
    }
    return false;
}

void Comando::formatear(string id, string type, string fs){
    if(fs==" ") fs = "2fs";
    cout<<"Se formaterá completamente en: "<<id<<endl;
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
    vector<string> id_spliteado = split_id(ids);
    int lon=0;
    while(lon<id_spliteado.size()){
        if(id==id_spliteado.at(lon)){
            lon+=1;
            break;
        }
        lon++;
    }

}

void Comando::reporte(string id, string ubicacion, string nombre){
    vector<string> id_spliteado = split_id(ids);
    int lon=0;
    while(lon<id_spliteado.size()){
        if(id==id_spliteado.at(lon)){
            cout<<"Se econtro el id:"<<id<<endl;
            if(nombre=="mbr"){
                cout<<"Se creará el reporte del mbr"<<endl;
                reporte_mbr(id,ubicacion);
            }else if(nombre=="disk"){
                cout<<"Se creará el reporte del disco"<<endl;
                reporte_disk(id, ubicacion);
            }
        }
        lon++;
    }
}
void Comando::reporte_mbr(string id, string ubicacion){
    vector<string> id_spliteado = split_id(ids);
    int lon=0;
    string ubi="";
    while(lon<id_spliteado.size()){
        if(id==id_spliteado.at(lon)){
            ubi=id_spliteado.at(lon+1);
            break;
        }
        lon++;
    }
    FILE *file;
    MBR mbr;
    file = fopen(ubi.c_str(),"r");
    if(file!=NULL){
        //comenzar a leer el disco
        crear_carpeta(ubicacion);
        string disk = "digraph G{\n";
        disk += "node[shape=record fillcolor=\"#FFEDBB\", style=filled]\n";
        disk += "label = \"MBR\"\n";
        disk += "bgcolor = \"#398D9C\"\n";
        disk += "subgraph G {\n";
        disk += "T[label=<\n";
        disk += "<table border=\"1\" cellborder =\"1\">\n";
        disk += "<tr><td bgcolor=\"yellow\">Reporte MBR</td></tr>\n";
        fseek(file,0,SEEK_SET);
        fread(&mbr, sizeof(mbr),1,file);
        disk += "<tr><td bgcolor=\"aquamarine\">mbr_tamaño</td><td bgcolor=\"aquamarine\">"+to_string(mbr.mbr_tamano)+"</td></tr>\n";
        disk += "<tr><td bgcolor=\"aquamarine\">mbr_fecha_de_creación</td><td bgcolor=\"aquamarine\">"+to_string(mbr.mbr_fecha_creacion)+"</td></tr>\n";
        disk += "<tr><td bgcolor=\"aquamarine\">dk_signature</td><td bgcolor=\"aquamarine\">"+to_string(mbr.mbr_dsk_signature)+"</td></tr>\n";
        if(mbr.mbr_partition_1.part_name[0]!='\0'){
            disk += "<tr><td bgcolor=\"yellow\">Partición</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_status</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_1.part_status)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_type</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_1.part_type)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_fit</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_1.part_fit)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_start</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_1.part_start)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_size</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_1.part_s)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_name</td><td bgcolor=\"darkseagreen3\">"+string(mbr.mbr_partition_1.part_name)+"</td></tr>\n";
        }
        if(mbr.mbr_partition_2.part_name[0]!='\0'){
            disk += "<tr><td bgcolor=\"yellow\">Partición</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_status</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_2.part_status)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_type</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_2.part_type)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_fit</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_2.part_fit)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_start</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_2.part_start)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_size</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_2.part_s)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_name</td><td bgcolor=\"darkseagreen3\">"+string(mbr.mbr_partition_2.part_name)+"</td></tr>\n";
        }
        if(mbr.mbr_partition_3.part_name[0]!='\0'){
            disk += "<tr><td bgcolor=\"yellow\">Partición</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_status</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_3.part_status)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_type</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_3.part_type)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_fit</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_3.part_fit)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_start</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_3.part_start)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_size</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_3.part_s)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_name</td><td bgcolor=\"darkseagreen3\">"+string(mbr.mbr_partition_3.part_name)+"</td></tr>\n";
        }
        if(mbr.mbr_partition_4.part_name[0]!='\0'){
            disk += "<tr><td bgcolor=\"yellow\">Partición</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_status</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_4.part_status)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_type</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_4.part_type)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_fit</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_4.part_fit)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_start</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_4.part_start)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_size</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_4.part_s)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_name</td><td bgcolor=\"darkseagreen3\">"+string(mbr.mbr_partition_4.part_name)+"</td></tr>\n";
        }
        disk += "</table>>];\n";
        disk += "}\n";
        disk += "}\n";
        ofstream table_disk;
        string cmd = "dot reporte_mbr.dot -o "+ubicacion+" -Tpng";
        table_disk.open("reporte_mbr.dot");
        table_disk << disk;
        table_disk.close();
        system(cmd.c_str());
    }else{
        cout<<"No se leyó el arvhivo, ruta:"<<ubi<<endl;
    }
}

void Comando::reporte_disk(string id, string ubicacion){
    vector<string> id_spliteado = split_id(ids);
    int lon=0;
    string ubi="";
    while(lon<id_spliteado.size()){
        if(id==id_spliteado.at(lon)){
            ubi=id_spliteado.at(lon+1);
            break;
        }
        lon++;
    }
    FILE *file;
    MBR mbr;
    file = fopen(ubi.c_str(),"r");
    if(file!=NULL){
        //comenzar a leer el disco
        crear_carpeta(ubicacion);
        string disk = "digraph G{\n";
        disk += "node[shape=record fillcolor=\"#FFEDBB\", style=filled]\n";
        disk += "label = \"Disk\"\n";
        disk += "bgcolor = \"#398D9C\"\n";
        disk += "subgraph G {\n";
        disk += "T[label=<\n";
        disk += "<table border=\"1\" cellborder =\"1\">\n";
        disk += "<tr><td bgcolor=\"yellow\">Reporte MBR</td></tr>\n";
        fseek(file,0,SEEK_SET);
        fread(&mbr, sizeof(mbr),1,file);
        if(mbr.mbr_partition_1.part_name[0]!='\0'){
            disk += "<tr><td bgcolor=\"yellow\">Partición</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_status</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_1.part_status)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_type</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_1.part_type)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_fit</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_1.part_fit)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_start</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_1.part_start)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_size</td><td bgcolor=\"darkseagreen3\">"+to_string(mbr.mbr_partition_1.part_s)+"</td></tr>\n";
            disk += "<tr><td bgcolor=\"darkseagreen3\">part_name</td><td bgcolor=\"darkseagreen3\">"+string(mbr.mbr_partition_1.part_name)+"</td></tr>\n";
        }
        disk += "</table>>];\n";
        disk += "}\n";
        disk += "}\n";
        ofstream table_disk;
        string cmd = "dot reporte_mbr.dot -o "+ubicacion+" -Tpng";
        table_disk.open("reporte_mbr.dot");
        table_disk << disk;
        table_disk.close();
        system(cmd.c_str());
    }else{
        cout<<"No se leyó el arvhivo, ruta:"<<ubi<<endl;
    }
}