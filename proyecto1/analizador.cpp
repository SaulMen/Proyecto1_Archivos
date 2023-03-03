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
            //cout<<cmdentrada.at(i)<<endl;
        }else{
            parametros.push_back(cmdentrada.at(i));
            //cout<<cmdentrada.at(i)<<endl;
        }
        
    }
    // Identificacion de comando, parametros y ejecucion
    return identificarParametros(comando, parametros);
}

string Analizador::identificarParametros(string comando, vector<string> parametros){
    string param = "";
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
                param = replace_txt(param, "\"", "");
                cmd.param.Tamano = param;
                cout<<param<<endl;
            } else if(param.find(">unit=") == 0){
                param = replace_txt(param, ">unit=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.Dimensional = param;
                cout<<param<<endl;
            }else if(param.find(">path=") == 0){
                param = replace_txt(param, ">path=", "");
                param = replace_txt(param, "\"", "");
                cmd.param.Ubicacion = param;
                cout<<param<<endl;
            }
        }
        //cmd.crearArchivo(cmd.param.Tamano,cmd.param.Dimensional,cmd.param.Ubicacion);
    }else if(comando == "fdisk")
    {
        cmd.param.Comando = "fdisk";
        cout<<"ejecutando: fdisk"<<endl;
    }else if(comando == "rep")
    {
        cmd.param.Comando = "rep";
        cout<<"ejecutando: rep"<<endl;
        //cmd.generarGrafo();
    }
    return "";
}