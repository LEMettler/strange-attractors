// ###############################
//
//  Created on:  Thu Sep 19 2024
//  By:  Lukas Mettler
//  https://github.com/LEMettler
//
// ###############################

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <cmath>
#include <string>
#include <tuple>
#include <iomanip>
#include <limits>
#include "../function.h"


using namespace std;


//const int N_COEFFICIENTS = 12;
//
//// system of equations
//pair<double, double> system_of_equations( vector<double>& a, double x, double y) {
//    double new_x = a[0] + a[1]*x + a[2]*x*x + a[3]*y +  a[4]*y*y + a[5]*x*y;
//    double new_y = a[6] + a[7]*x + a[8]*x*x + a[9]*y + a[10]*y*y + a[11]*x*y;
//    return {new_x, new_y};
//}



tuple<vector<double>, vector<double>, vector<double>> find_attractor(int n_steps) {
    random_device rd; // Will be used to obtain a seed for the random number engine
    mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    uniform_real_distribution<> disStart(-0.5, 0.5); // rng for x, y starts
    uniform_real_distribution<> disCoeff(-2.0, 2.0); // rng for coefficients

    double x_max = 1.e10;
    double y_max = 1.e10;
    double dx_min = 1. / 1.e10;
    double dy_min = 1. / 1.e10;

    //lists of x,y that get returned
    vector<double> x_list;
    vector<double> y_list;

    //list of coeffs that get returned
    vector<double> coeffs(N_COEFFICIENTS);


    bool found = false;
    while (found == false) {

        // generate main start point
        double x = disStart(gen);
        double y = disStart(gen);
        x_list = {x};
        y_list = {y};


        // generate offset start point
        double x_alt = x + disStart(gen)/1000.0;
        double y_alt = y + disStart(gen)/1000.0;
        // distance alt - main
        double d0 = sqrt((x_alt - x)*(x_alt - x) + (y_alt - y)*(y_alt - y));


        // generate coefficents of SOE
        for (int i = 0; i < coeffs.size(); i++) {
            coeffs[i] = disCoeff(gen);
        } 

        bool converging = false;
        bool diverging = false;
        double lyapunov = 0;

        // evaluation loop
        for (int i = 0; i < n_steps; i++) {

            auto [new_x, new_y] = system_of_equations(coeffs, x, y);

            // rule of divergence to +-inf
            if (abs(new_x) > x_max or abs(y) > y_max) {
                diverging = true;
                break; //break out of for loop
            }
            //rule out convergence to single point
            if (abs(new_x-x) < dx_min and abs(y-y) < dy_min) {
                converging = true;
                break; //break out of for loop
            }

            // check chaotic behaviour
            if (i > 1000) {
                // calculate a new offset point
                auto [new_x_alt, new_y_alt] = system_of_equations(coeffs, x_alt, y_alt);
                
                // chaos characterization factor
                double d = sqrt((new_x_alt - new_x)*(new_x_alt - new_x) + (new_y_alt - new_y)*(new_y_alt - new_y));
                lyapunov += log(abs(d/d0));

                //rescale
                x_alt = new_x + (new_x_alt - new_x)*d0/d;
                y_alt = new_y + (new_y_alt - new_y)*d0/d;
            }

            x = new_x;
            y = new_y;

            // add new point to lists
            x_list.push_back(x);
            y_list.push_back(y);

        }
        
        // was a valid system found
        if ((converging == false) and (diverging == false) and (lyapunov >= 10.)) {
            found = true; // -> break out of while loop
        }
    }

    // a solution was found and the datapoints are returned outside of the while loop
    return {x_list, y_list, coeffs};
}




// store coefficients in file
void store_coeffs( string& filename, vector<double>& coeffs) {

    setprecision(numeric_limits<double>::max_digits10);
     
    ofstream output_file(filename);

    for (size_t i = 0; i < coeffs.size(); i++) {
        output_file << coeffs[i] << endl;
    }
    output_file.close();

    cout << "Coefficients have been written to: " << filename << endl;
}




// store datapoints in file
void store_points( string& filename,  vector<double>& x_list,  vector<double>& y_list) {
    
    setprecision(numeric_limits<double>::max_digits10);
    
    ofstream output_file(filename);

    output_file << "x, y" << endl;

    for (size_t i = 0; i < x_list.size(); i++) {
        output_file << x_list[i] << ", " << y_list[i] << endl;
    }
    output_file.close();

    cout << "Datapoints have been written to: " << filename << endl;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// main function can take arguments when calling program
int main(int argc, char* argv[]) {
    string run_name;
    int n_attractors;
    int n_steps;

    if (argc == 4) {
        // correct number of arguments
        run_name = argv[1];
        try {
            n_attractors = stoi(argv[2]);
            n_steps = stoi(argv[3]);
        } catch (const invalid_argument& e) {
            cerr << "Error: Invalid number format for attractors or steps." << endl;
            return 1;
        } catch (const out_of_range& e) {
            cerr << "Error: Number out of range for attractors or steps." << endl;
            return 1;
        }

        // output input values
        cout << "Run name: " << run_name << endl;
        cout << "Number of attractors: " << n_attractors << endl;
        cout << "Number of steps: " << n_steps << endl;


    } else {
        // input via user
        cout << "Run name: ";
        cin >> run_name;

        cout << "Number of attractors: ";
        cin >> n_attractors;

        cout << "Number of steps: ";
        cin >> n_steps;
    }


    
    // main loop
    for (int i = 0; i < n_attractors; i++) {

        auto [x_list, y_list, coeffs] = find_attractor(n_steps);
        
        string coeff_filename = "data/" + run_name + "/coeffs/" + to_string(i) + ".txt";
        string data_filename = "data/" + run_name + "/points/" + to_string(i) + ".txt";

        store_coeffs(coeff_filename, coeffs);
        store_points(data_filename, x_list, y_list);
    }

    return 0;
}
