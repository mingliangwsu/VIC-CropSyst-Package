#ifndef PUBTOOLS_H
#define PUBTOOLS_H

//#ifndef __BCPLUSPLUS__
//RLN This is causing problems, mixing C and C++


#include "stdio.h"
#include "stdlib.h"
#include <string>
#include <math.h>
#if __GNUC__
#include <cxxabi.h>
#endif


#include <sstream>
#include <iostream>
#include "corn/math/compare.hpp"
#include <vector>
#include <limits>
//#endif

extern void nrerror(char error_text[]);

//#ifndef __BCPLUSPLUS__
//RLN This is causing problees, mixing C and C++



extern double SumOfArray(double *data_array,double invalid_data,int members);
extern double MeanOfArray(double *data_array,double invalid_data,int members);
extern double MaxOfArray(double *data_array,double invalid_data,int members);
extern double MinOfArray(double *data_array,double invalid_data,int members);
extern void removeWhitespace(std::string& str);
//______________________________________________________________________________
template <class array_type>
void delete_1d_array(array_type *p)
{
    delete[] p;
}
//______________________________________________________________________________
template <class array_type>
void delete_2d_array(array_type **p,int rows)
{
    for (int i = 0; i < rows; i++)
        delete[] p[i];
    delete[] p;
}
//______________________________________________________________________________
template <class array_type>
void delete_3d_array(array_type ***p,int d1,int d2)
{
    for (int i = 0; i<d1; i++) {
        for (int j = 0; j<d2; j++) {
            delete[] p[i][j];
        }
        delete[] p[i];
    }
    delete[] p;
}
//______________________________________________________________________________
template <class array_type>
void delete_4d_array(array_type ***p,int d1,int d2,int d3)
{
    for (int i = 0; i < d1; i++) {
        for (int j = 0; j < d2; j++) {
            for (int k = 0; k < d3; k++) {
                delete[] p[i][j][k];
            }
            delete[] p[i][j];
        }
        delete[] p[i];
    }
    delete[] p;
}
//______________________________________________________________________________
template <class array_type> array_type *alloc_1d_array(int rows, const char* varname)
{
    array_type *pdata = new array_type[rows];
    return pdata;
}
//______________________________________________________________________________
template <class array_type> array_type **alloc_2d_array(int rows, int columns, const char* varname)
{
    array_type **pdata = new array_type*[rows];
    for (int i = 0; i < rows; i++) {
        pdata[i] = new array_type[columns];
    }
    return pdata;
}
//______________________________________________________________________________
template <class array_type> array_type ***alloc_3d_array(int d1, int d2, int d3, const char* varname)
{
    array_type ***pdata = new array_type**[d1];
    for (int i = 0; i < d1; i++) {
        pdata[i] = new array_type*[d2];
        for (int j = 0; j < d2; j++) {
            pdata[i][j] = new array_type[d3];
       }
    }
    return pdata;
}
//______________________________________________________________________________
template <class array_type> array_type ****alloc_4d_array(int d1, int d2, int d3, int d4, const char* varname)
{
    array_type ****pdata = new array_type ***[d1];
    for (int i = 0; i < d1; i++) {
        pdata[i] = new array_type **[d2];
        for (int j = 0; j < d2; j++) {
            pdata[i][j] = new array_type *[d3];
            for (int k = 0; k < d3; k++) {
                pdata[i][j][k] = new array_type[d4];
            }
        }
    }
    return pdata;
}
//______________________________________________________________________________
template <class array_type> void copy_2d_array(array_type **from,array_type **to,int rows,int cols)
{
    #ifdef __linux__
    //190129 complains when compiled under Windows
    #pragma omp parallel for num_threads(NUMCORES_TO_USE)
    #endif
    for (long ij = 0; ij < rows * cols; ij++) {
        long i = (long)(ij / cols);
        long j = ij % cols;
        to[i][j] = from[i][j];
    }
}
//______________________________________________________________________________
template <class array_type> void copy_1d_array(array_type *from,array_type *to,int rows)
{
    #ifdef __linux__
    //190129 complains when compiled under Windows
    #pragma omp parallel for num_threads(NUMCORES_TO_USE)
    #endif
    for (int i = 0; i < rows; i++) {
            to[i] = from[i]; 
    }
}
//______________________________________________________________________________
template <class array_type>
array_type MaxOfArray(const std::vector<array_type> &data_array,array_type invalid_data,int members)
{
    //Calculate the maximum from array
    //invalid_data shows the invalid data if not valid
    array_type temp = std::numeric_limits<array_type>::min();

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
template <class array_type>
array_type SumOfArray(const std::vector<array_type> &data_array,array_type invalid_data,int members)
{
    //Calculate the sum from array
    //invalid_data shows the invalid data if not valid
    array_type temp(0);
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
template <class array_type>
double MeanOfArray(const std::vector<array_type> &data_array,array_type invalid_data,int members)
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
template <class array_type>
array_type MinOfArray(const std::vector<array_type> &data_array,array_type invalid_data,int members)
{
    //Calculate the minimum from array
    //invalid_data shows the invalid data if not valid
    array_type temp(std::numeric_limits<array_type>::max());  //LML 150205 need check!!!
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
//#endif


#endif // PUBTOOLS_H
