#include <string>
#include "stdio.h"
#include "stdlib.h"
#include <iostream>
#include <cmath>
#include <cfloat>
#include <time.h>
#include "pubtools.h"
#ifdef __BCPLUSPLUS__
#  include <limits>
#endif
#include <cmath>            //fabs problem
#ifndef MAXSTRING
#define MAXSTRING 2048
#endif
//160926LML
//160926LMLvoid nrerror(char error_text[MAXSTRING])
//160926LML/* Numerical Recipes standard error handler */
//160926LML{
//160926LML    std::cerr<<"Model run-time error...\n";
//160926LML    std::cerr<<error_text<<std::endl;
//160926LML    std::cerr<<"...now exiting to system...\n";
//160926LML    exit(0);
//160926LML};
//______________________________________________________________________________
double MeanOfArray(double *data_array,double invalid_data,int members)
{
    //Calculate the average from array
    //invalid_data shows the invalid data if not valid
    double temp(0);
    for (int i = 0; i < members; i++) {
        if (!CORN::is_approximately<double>(data_array[i],invalid_data)) {
            temp += data_array[i]/members;
        } else {
            temp = invalid_data;
            i = members;
        }
    }
    return temp;
}
//______________________________________________________________________________
double SumOfArray(double *data_array,double invalid_data,int members)
{
    //Calculate the sum from array
    //invalid_data shows the invalid data if not valid
    double temp(0);
    for (int i = 0; i < members; i++) {
        if (!CORN::is_approximately<double>(data_array[i],invalid_data)) {
            temp += data_array[i];
        } else {
            temp = invalid_data;
            i = members;
        }
    }
    return temp;
}
//______________________________________________________________________________
double MaxOfArray(double *data_array,double invalid_data,int members)
{
    //Calculate the maximum from array
    //invalid_data shows the invalid data if not valid
    double temp = std::numeric_limits<int>::min();
    for (int i = 0; i < members; i++) {
        if (!CORN::is_approximately<double>(data_array[i],invalid_data)) {
            if (temp<data_array[i]) temp = data_array[i];
        } else {
            temp = invalid_data;
            i = members;
        }
    }
    return temp;
}
//______________________________________________________________________________
double MinOfArray(double *data_array,double invalid_data,int members)
{
    //Calculate the minimum from array
    //invalid_data shows the invalid data if not valid
    double temp(std::numeric_limits<int>::max());
    for (int i = 0; i < members; i++) {
        if (!CORN::is_approximately<double>(data_array[i],invalid_data)) {
            if (temp>data_array[i]) temp = data_array[i];
        } else {
            temp = invalid_data;
            i = members;
        }
    }
    return temp;
}
//______________________________________________________________________________
void removeWhitespace(std::string& str)
{
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == ' ' || str[i] == '\n' || str[i] == '\t' || str[i] == '\r') {
            str.erase(i, 1);
            i--;
        }
    }
}
