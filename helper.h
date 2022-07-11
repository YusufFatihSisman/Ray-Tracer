#ifndef HELPER_H
#define HELPER_H
#include <string>
#include <vector>
#include <limits>

using namespace std;

template <typename T>
void parse_string_and_assign(string str, T* array);

template <typename T>
void parse_string_and_assign_vector(string str, vector<T>& vec);
#endif