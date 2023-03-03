#ifndef ANALIZADOR_H
#define ANALIZADOR_H

#include "comando.cpp"
#include <algorithm>
#include <string>
#include <vector>
#include <sstream>
#include<bits/stdc++.h>


class Analizador
{
    public:
        Comando cmd;
        vector<string> split_txt(string entrada);
        string replace_txt(string str, const string& from, const string& to);
        string analizar(string entrada);
        string identificarParametros(string comando,vector<string> parametros);
};

#endif