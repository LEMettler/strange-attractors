// ###############################
//
//  Created on:  Thu Sep 19 2024
//  By:  Lukas Mettler (lukas.mettler@student.kit.edu)
//  https://github.com/LEMettler
//
// ###############################

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <cmath>
#include <string>
#include <tuple>
#include <iomanip>
#include <limits>
#include "../function.h"


using namespace std;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//// system of equations
//pair<double, double> system_of_equations( vector<double>& a, double x, double y) {
//    double new_x = a[0] + a[1]*x + a[2]*x*x + a[3]*y +  a[4]*y*y + a[5]*x*y;
//    double new_y = a[6] + a[7]*x + a[8]*x*x + a[9]*y + a[10]*y*y + a[11]*x*y;
//    return {new_x, new_y};
//}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


pair<vector<double>, vector<double>> further_calculate_attractor(int n_steps, vector<double>& x_list, vector<double>& y_list, vector<double>& coeffs) {
    
    double x_max = 1.e10;
    double y_max = 1.e10;
    double dx_min = 1. / 1.e10;
    double dy_min = 1. / 1.e10;


    // generate main start point
    double x = x_list[0];
    double y = y_list[0];

    // evaluation loop
    for (int i = 0; i < n_steps; ++i) {

        auto [new_x, new_y] = system_of_equations(coeffs, x, y);

        // rule of divergence to +-inf
        if (abs(new_x) > x_max or abs(y) > y_max) {
            cout << "Diverging to +- inf after " << i << " steps!" << endl;
            break; //break out of for loop
        }
        //rule out convergence to single point
        if (abs(new_x-x) < dx_min and abs(y-y) < dy_min) {
            cout << "Converging to single point after " << i << " steps!" << endl;
            break; //break out of for loop
        }
    
        x = new_x;
        y = new_y;

        // add new point to lists
        x_list.push_back(x);
        y_list.push_back(y);

    }

    return {x_list, y_list};
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// read coefficients from file
vector<double> read_coefficients(const string& filename) {
    vector<double> a;
    
    string line;
    ifstream input_file(filename);

    while (getline(input_file, line)) {
        a.push_back(stod(line));
    }

    input_file.close();
    return a;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



// read already calculated data from file
pair<vector<double>, vector<double>> read_previous_points(const string& filename) {
    
    vector<double> x;
    vector<double> y;

    string line;
    ifstream input_file(filename);

    // skip the first line
    getline(input_file, line);

    // read data into x, y for all following lines
    while (getline(input_file, line)) {
        stringstream ss(line);
        string x_str, y_str;

        if (getline(ss, x_str, ',') and getline(ss, y_str)) {
            try {
                x.push_back(stod(x_str));
                y.push_back(stod(y_str));
            } catch (const invalid_argument& e) {
                cerr << "Invalid number format: " << line << endl;
            } catch (const out_of_range& e) {
                cerr << "Number out of range: " << line << endl;
            }
        }
    }
    
    input_file.close();
    
    return {x, y};
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// read already calculated data from file
pair<vector<double>, vector<double>> read_previous_start(const string& filename) {
    
    vector<double> x;
    vector<double> y;

    string line;
    ifstream input_file(filename);

    // skip the first line
    getline(input_file, line);


    // get the first data line
    getline(input_file, line);
    stringstream ss(line);
    string x_str, y_str;
    // seperation by ","
    getline(ss, x_str, ',');
    getline(ss, y_str);
    x.push_back(stod(x_str));
    y.push_back(stod(y_str));

    input_file.close();
    
    return {x, y};
}



// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// store datapoints in file
void store_points( string& filename,  vector<double>& x_list,  vector<double>& y_list) {

    setprecision(numeric_limits<double>::max_digits10);

    ofstream output_file(filename);

    output_file << "x, y" << endl;

    for (size_t i = 0; i < x_list.size(); ++i) {
        output_file << x_list[i] << ", " << y_list[i] << endl;
    }
    output_file.close();

    cout << "Datapoints have been written to: " << filename << endl;
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// main function can take arguments when calling program
int main(int argc, char* argv[]) {
    string run_name;
    int idx_attractor;
    int n_steps;

    if (argc == 4) {
        // correct number of arguments
        run_name = argv[1];
        try {
            idx_attractor = stoi(argv[2]);
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
        cout << "Index of attractor: " << idx_attractor << endl;
        cout << "Number of steps: " << n_steps << endl;


    } else {
        // input via user
        cout << "Run name: ";
        cin >> run_name;

        cout << "Index of attractor: ";
        cin >> idx_attractor;

        cout << "Number of steps: ";
        cin >> n_steps;
    }

    
    // read in previous
    string input_points_filename = "data/" + run_name + "/points/" + to_string(idx_attractor) + ".txt";
    string input_coeffs_filename = "data/" + run_name + "/coeffs/" + to_string(idx_attractor) + ".txt";
    
    auto [x_list, y_list] = read_previous_points(input_points_filename);
    //auto [x_list, y_list] = read_previous_start(input_points_filename);
    
    vector<double> coeffs = read_coefficients(input_coeffs_filename);
    
    // calculate new
    auto [x_list_exp, y_list_exp] = further_calculate_attractor(n_steps, x_list, y_list, coeffs);

    // store new
    string output_points_filename = "data/" + run_name + "/points/" + to_string(idx_attractor) + "exp.txt";
    store_points(output_points_filename, x_list_exp, y_list_exp);
    

    return 0;
}
