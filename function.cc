// ###############################
//
//  Created on:  Thu Sep 19 2024
//  By:  Lukas Mettler
//  https://github.com/LEMettler
//
// ###############################

#include <vector>
#include <tuple>
#include "function.h"

using namespace std;


// system of equations
pair<double, double> system_of_equations( vector<double>& a, double x, double y) {
    double new_x = a[0] + a[1]*x + a[2]*x*x + a[3]*y +  a[4]*y*y + a[5]*x*y;
    double new_y = a[6] + a[7]*x + a[8]*x*x + a[9]*y + a[10]*y*y + a[11]*x*y;
    return {new_x, new_y};
}
