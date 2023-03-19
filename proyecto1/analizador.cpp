#include "analizador.h"

vector<string> Analizador::split_txt(string text){
    stringstream text_to_split(text);
    string segment;
    vector<string> splited;
    while(std::getline(text_to_split, segment, ' ')){
        splited.push_back(segment);
    }
    return splited;
}

string Analizador::replace_txt(string str, const string& from, const string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

string Analizador::analizar(string entrada){
    string entradacmd = entrada;
    // Transformacion de toda la entrada a minusculas
    transform(entradacmd.begin(), entradacmd.end(), entradacmd.begin(), ::tolower);
    vector<string> cmdentrada = split_txt(entradacmd);
    vector<string> parametros;
    string comando = "";
    for(int i = 0; i< cmdentrada.size(); i++){
        if(i == 0){
            comando = cmdentrada.at(i);
            //if(comando!="execute") comando.erase(0,1);
            if(comando=="\n"||comando==" \n"||comando==" ") continue;
            if(comando[comando.size()-1]=='\n') comando.pop_back();
            //cout<<"comando: "<<comando<<endl;
        }else{
            if(cmdentrada.at(i).length()>0){
                if(cmdentrada.at(i).substr(cmdentrada.at(i).length()-1)=="\n"){
                    cmdentrada.at(i).pop_back();
                }
            }
            

            parametros.push_back(cmdentrada.at(i));
        }
        
    }
    // Identificacion de comando, parametros y ejecucion
    return identificarParametros(comando, parametros);
}

string Analizador::obtenerRuta(string param, vector<string> parametros){
    //param -> ruta del archivo
    string ubi_unidad = "";
    int posicion;
    if(param.find("\"") == 0){
        for (int i=0;i<parametros.size();i++){
            if(parametros.at(i).find(">path=\"") == 0){
                posicion = i;
            }
        }
        while(true){
            if(parametros.at(posicion).substr(parametros.at(posicion).length()-2)=="\"\n"
            || parametros.at(posicion).substr(parametros.at(posicion).length()-1)=="\""){
                ubi_unidad += parametros.at(posicion);
                break;
            }else{
                ubi_unidad += parametros.at(posicion) + " ";
                posicion ++;
            }
        }
    }
    ubi_unidad = replace_txt(ubi_unidad, ">path=", "");
    if(param.find("\"") == 0){
        if(parametros.at(posicion).substr(parametros.at(posicion).length()-2)=="\"\n"){
            ubi_unidad.erase(0,1); ubi_unidad.pop_back(); ubi_unidad.pop_back();
        }else{
            ubi_unidad.erase(0,1); ubi_unidad.pop_back();
        }
        return ubi_unidad;
    }else{
        return param;
    }
}

string Analizador::identificarParametros(string comando, vector<string> parametros){
    string param = "";

    if(comando=="\n"||comando==" \n"||comando==" ") return "";
    //cout<<"Entra el comando: "<<comando<<endl;
    if(comando == "execute"){
        cmd.param.Comando = "execute";
        cout<<"ejecutando: execute"<<endl;
        // Ejecucion de metodo
        for(int i=0; i<parametros.size(); i++){
            param = parametros.at(i);
            //cout<<"-->:"<<param<<endl;
            if(param.find(">path=") == 0){
                //cout<<"-->:"<<param<<endl;
                param = replace_txt(param, ">path=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.Ubicacion = param;
            }else if(param.find('"') != string::npos){
                param = replace_txt(param, "\"", "");
                cmd.param.Ubicacion += " " + param;
            }
        }

        return cmd.param.Ubicacion;//cmd.identificacionCMD(cmd.param);
    }else if(comando == "mkdisk")
    {
        cmd.param.Comando = "mkdisk";
        cout<<"ejecutando: mkdisk"<<endl;
        for(int i=0; i<parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">size=") == 0){
                param = replace_txt(param, ">size=", "");
                //param = replace_txt(param, "\"", "");
                cmd.param.Tamano = param;
                cout<<" -tamaño:"<<param;
            } else if(param.find(">unit=") == 0){
                param = replace_txt(param, ">unit=", "");
                //param = replace_txt(param, "\"", "");
                cmd.param.Dimensional = param;
                cout<<" -unidad: "<<param;
            }else if(param.find(">path=") == 0){
                param = replace_txt(param, ">path=", "");
                cmd.param.Ubicacion = obtenerRuta(param, parametros);
                cout<<" -ubicación: "<<cmd.param.Ubicacion;
            }else if(param.find(">fit=")==0){
                param = replace_txt(param, ">fit=", "");
                //param = replace_txt(param, "\"", "");
                cmd.param.Fit = param;
                cout<<" -fit: "<<param;
            }
        }
        cout<<endl<<endl;
        cmd.identificarCMD(cmd.param);
    }else if(comando == "rmdisk"){
        cmd.param.Comando = "rmdisk";
        cout<<"ejecutando rmdisk"<<endl;

        for(int i=0;i<parametros.size();i++){
            param = parametros.at(i);
            if(param.find(">path=") == 0){
                param = replace_txt(param, ">path=", "");
                cmd.param.Ubicacion = obtenerRuta(param, parametros);
                cout<<" -ubicacion: "<<cmd.param.Ubicacion<<endl;
            }
        }
        cmd.identificarCMD(cmd.param);
    }else if(comando == "fdisk")
    {
        cmd.param.Comando = "fdisk";
        cout<<"ejecutando: fdisk"<<endl;
        for(int i=0; i<parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">size=") == 0){
                param = replace_txt(param, ">size=", "");
                cmd.param.Tamano = param;
                cout<<" -tamaño: "<<param;
            }else if(param.find(">unit=") == 0){
                param = replace_txt(param, ">unit=", "");
                cmd.param.Unit = param;
                cout<<" -unidad: "<<param;
            }else if(param.find(">path=") == 0){
                param = replace_txt(param, ">path=", "");
                cmd.param.Ubicacion = obtenerRuta(param, parametros);
                cout<<" -ubicacion: "<<cmd.param.Ubicacion<<endl;
            }else if(param.find(">fit=")==0){
                param = replace_txt(param, ">fit=", "");
                cmd.param.Fit = param;
                cout<<" -fit: "<<param;
            }else if(param.find(">name=")==0){
                param = replace_txt(param, ">name=", "");
                cmd.param.Nombre = param;
                cout<<" -nombre: "<<param;
            }else if(param.find(">type=")==0){
                param = replace_txt(param, ">type=", "");
                cmd.param.Tipo = param;
                cout<<" -type: "<<param;
            }else if(param.find(">delete=")==0){
                param = replace_txt(param, ">delete=", "");
                cmd.param.Delete = param;
                cout<<" -delete: "<<param;
            }else if(param.find(">add=")==0){
                param = replace_txt(param, ">add=", "");
                cmd.param.Add = param;
                cout<<" -add: "<<param;
            }
        }
        cout<<endl<<endl;
        cmd.identificarCMD(cmd.param);

    }else if(comando == "mount"){
        cmd.param.Comando = "mount";
        cout<<"ejecutando: mount"<<endl;
        for(int i=0; i<parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">name=") == 0){
                param = replace_txt(param, ">name=", "");
                cmd.param.Nombre = param;
                cout<<" -nombre: "<<param;
            }else if(param.find(">path=") == 0){
                param = replace_txt(param, ">path=", "");
                cmd.param.Ubicacion = obtenerRuta(param, parametros);
                cout<<" -ubicacion: "<<cmd.param.Ubicacion;
            }
        }
        cout<<endl<<endl;
        cmd.identificarCMD(cmd.param);
    }else if(comando == "unmount"){
        cmd.param.Comando = "unmount";
        cout<<"ejecutando: unmount"<<endl;
        for(int i=0; i<parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">id=") == 0){
                param = replace_txt(param, ">id=", "");
                cmd.param.Id = param;
                cout<<"Id: "<<param;
            }
        }
        cout<<endl<<endl;
        cmd.identificarCMD(cmd.param);
    }else if(comando == "mkfs"){
        cmd.param.Comando = "mkfs";
        cout<<"ejecutando: mkfs"<<endl;
        for(int i=0; i<parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">id=") == 0){
                param = replace_txt(param, ">id=", "");
                cmd.param.Id = param;
                cout<<" -id: "<<param;
            }else if(param.find(">path=") == 0){
                param = replace_txt(param, ">path=", "");
                cmd.param.Ubicacion = obtenerRuta(param, parametros);
                cout<<" -ubicacion: "<<param;
            }else if(param.find(">type=") == 0){
                param = replace_txt(param, ">type=", "");
                cmd.param.Tipo = param;
                cout<<" -tipo: "<<param;
            }
        }
        cout<<endl<<endl;
        cmd.identificarCMD(cmd.param);
    }else if(comando=="pause"){
        cout<<"ejecutando: pause"<<endl;
        cout << "   (Enter para continuar)";
        char cadena;
        bool check = true;
        while(cin.get(cadena) && check)
        {
            if( check = (cadena != '\n') )
            {
                cout << cadena << '\n';
            }
        }
        cout<<"Continuando ejecución del programa"<<endl;
    }else if(comando == "rep")
    {
        cmd.param.Comando = "rep";
        cout<<"ejecutando: rep"<<endl;
        for(int i=0; i<parametros.size(); i++){
            param = parametros.at(i);
            if(param.find(">id=") == 0){
                param = replace_txt(param, ">id=", "");
                cmd.param.Id = param;
                cout<<" -id: "<<param;
            }else if(param.find(">path=") == 0){
                param = replace_txt(param, ">path=", "");
                cmd.param.Ubicacion = obtenerRuta(param, parametros);
                cout<<" -ubicacion: "<<param;
            }else if(param.find(">name=") == 0){
                param = replace_txt(param, ">name=", "");
                cmd.param.Nombre = param;
                cout<<" -nombre: "<<param;
            }
        }
        cout<<endl<<endl;
        cmd.identificarCMD(cmd.param);
    }
    else{
        if(comando.size()!=0)
        cout<<"Error al reconocer un comando ->"<<comando<<endl;
    }
    cmd.param.Add=" ";cmd.param.Comando=" ";cmd.param.Delete=" ";cmd.param.Dimensional=" ";
    cmd.param.Fit=" ";cmd.param.Fs=" ";cmd.param.Id=" ";cmd.param.Nombre=" ";
    cmd.param.Tamano=" ";cmd.param.Tipo=" ";cmd.param.Ubicacion=" ";cmd.param.Unit=" ";
    return "";
}