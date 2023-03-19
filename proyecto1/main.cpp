#include <iostream>
#include "analizador.cpp"
#include <sys/stat.h>

using namespace std;

int main()
{
    
    Analizador analisis;
    string entradacmd = "", es_execute="";
    char enter;

    while(entradacmd != "exit")
    {
        cout << "   Proyecto:~$ ";
        getline(cin,entradacmd);
        es_execute = analisis.analizar(entradacmd);//Si es execute me devuela el path
        cout<<"Ruta del archivo:"<<es_execute<<endl<<endl;
        if(es_execute!=""){
            //cout<<"Es comando ejecutar...|"<<es_execute<<"|"<<endl;

            FILE *file;
            file = fopen(es_execute.c_str(), "r");
            char cont[300];
            string coment="";
            string linea;

            if (file != NULL) {

                while (fgets(cont, 300, file) != NULL) {
                    if(string(cont).find("#")==0){
                        coment=string(cont);
                        coment.pop_back();
                        cout<<"comentario: "<<coment<<endl<<endl;
                    }else{
                        analisis.analizar(string(cont));
                    }
                }
                
                fclose(file);
            }else{
                cout<<"Error al leer el archivo"<<endl;
            }
            
        }else{
            cout<<"No leí el execute"<<endl;
            exit(EXIT_FAILURE);
            FILE *file;
            string ruta = "/home/hola/jafet"; //nombre a crear la ruta
            string nuevo_archivo = "prueba.txt";

            file = fopen(es_execute.c_str(), "r");
            if (file != NULL) {
                cout<<"hola jeje"<<endl;
                fclose(file);
            }else{ // si no me abrió la direccion es porque no existe el directorio y se crea
            //Si el directorio no existe crear un array de string que a la vez me splitee
            // por diagonales y asi obtener cada carpeta, pero eliminar la ultima
            //ya que esa contendrá el archivo con extension
                cout<<"la ruta del archivo no existe\nCreando carpeta"<<endl;
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

            //file = fopen("/home/hola/prueba.txt", "a+");//Se vuelve a intentar y en teoría
            //fclose(file);                //debería funcioanar y a su paso crear el archivo
        }

    }

    return 0;
}