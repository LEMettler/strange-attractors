// ###############################
//
//  Created on:  Thu Sep 19 2024
//  By:  Lukas Mettler (lukas.mettler@student.kit.edu)
//  https://github.com/LEMettler
//
// ###############################

#ifndef FUNCTION_H
#define FUNCTION_H

#include <vector>
#include <utility>
using namespace std;

const int N_COEFFICIENTS = 12;
pair<double, double> system_of_equations(vector<double>& a, double x, double y);

#endif // FUNCTION_H