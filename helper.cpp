#include "helper.h"
#include <string>
#include <stdlib.h>
#include <bits/stdc++.h>

using namespace std;

template <typename T> 
void parse_string_and_assign(string str, T* array){
    stringstream ss(str);
    string word;
    int j = 0;
    while (ss >> word) {
        if(typeid(T) == typeid(int))
            array[j] = atoi(word.c_str());
        else if(typeid(T) == typeid(double))
            array[j] = atof(word.c_str());
        j++;
    }
}

template <typename T> 
void parse_string_and_assign_vector(string str, vector<T>& vec){
    stringstream ss(str);
    string word;
    while (ss >> word) {
        if(typeid(T) == typeid(int))
            vec.push_back(atoi(word.c_str()));
        else if(typeid(T) == typeid(double))
            vec.push_back(atof(word.c_str()));
    }
}

template void parse_string_and_assign<int>(string str, int* arr);
template void parse_string_and_assign<double>(string str, double* arr);
template void parse_string_and_assign_vector<int>(string str, vector<int>& vec);
template void parse_string_and_assign_vector<double>(string str, vector<double>& vec);
