#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>
#include <cstdlib>
#include <chrono>
#include <omp.h>
#include <string>
using namespace std;

const double T = .001;
const double G = 6.673e-11;
const double M = 1.0;

struct body {
    double mass = M;
    double position[2];
    double velocity[2] = {0, 0};
};

void initBodies(vector<body> &bodies);
void printBodies(vector<body> bodies);
vector<double> force(body b1, body b2);
double norm(vector<double> v1);
void v_divide(vector<double> &v, double s);

int main(int argc, char *argv[]){

    //auto start = chrono::high_resolution_clock::now();
    double start = omp_get_wtime();

    int n = atoi(argv[1]);
    int procs = atoi(argv[2]);
    int k = 6;

    vector<body> bodies(n);
    initBodies(bodies);

    //declare our vars up here to keep them private between threads
    int i, j;
    double x_force, y_force, xj_force, yj_force, a_jx, a_jy, a_x, a_y;

    for(int t = 0; t < k; t++){

        #pragma omp parallel num_threads(procs)
        #pragma omp for private(j, x_force, y_force, xj_force, yj_force, a_jx, a_jy, a_x, a_y)
        for(i=0; i < bodies.size(); i++){

            x_force = 0.0;
            y_force = 0.0;
            xj_force = 0.0;
            yj_force = 0.0;

            for(j=i; j < bodies.size(); j++){
                if(i == j) continue;
                vector<double> f = force(bodies[i], bodies[j]);
                //allops++;
                x_force += f[0];
                y_force += f[1];

                //apply opposite force to body j
                xj_force -= f[0];
                yj_force -= f[1];

                //update body j before we exit the inner loop
                a_jx = xj_force / bodies[j].mass;
                a_jy = yj_force / bodies[j].mass;

                //update velocity then position
                bodies[j].velocity[0] += T * a_jx;
                bodies[j].velocity[1] += T * a_jy;
                bodies[j].position[0] += T * bodies[j].velocity[0];
                bodies[j].position[1] += T * bodies[j].velocity[1];
            }

            //calc acceleration
            a_x = x_force / bodies[i].mass;
            a_y = y_force / bodies[i].mass;

            //update velocity then position
            bodies[i].velocity[0] += T * a_x;
            bodies[i].velocity[1] += T * a_y;
            bodies[i].position[0] += T * bodies[i].velocity[0];
            bodies[i].position[1] += T * bodies[i].velocity[1];

        }
    }

    //auto stop = chrono::high_resolution_clock::now();
    //auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);

    //double time = duration.count() / 10000000.0;

    double end = omp_get_wtime();
    double time = end - start;
    
    double p = ((k * (n * n))*1.0) / time;
    cout << fixed << n << ", " << procs << ", " << p << endl;
    //cout << "Time taken by function: "<< duration.count() << " microseconds" << endl;
    //cout << n << ", " << duration.count() << endl;

    //printBodies(bodies);
    return 0;

}

vector<double> force(body b1, body b2){
    vector<double> r_ij;// = { b2.position[0] - b1.position[0], b2.position[1] - b1.position[1] };
    r_ij.push_back(b2.position[0] - b1.position[0]);
    r_ij.push_back(b2.position[1] - b1.position[1]);

    //take norm of r_ij to get denom for f_ij
    double r_norm = norm(r_ij);
    double denom = (r_norm * r_norm * r_norm);

    //scale our numerator vector
    double scalar = (G * b1.mass * b2.mass) * 1.0;
    vector<double> scaled_r;//= { r_ij[0] * scalar, r_ij[1] * scalar };
    scaled_r.push_back(r_ij[0] * scalar);
    scaled_r.push_back(r_ij[1] * scalar);

    //divide vector by scalar
    v_divide(scaled_r, denom);

    return scaled_r;

}

double norm(vector<double> v1){
    double norm = 0.0;
    for(int i=0; i < v1.size(); i++){
        norm += (v1[i] * v1[i]);
    }
    return sqrt(norm);
}

void v_divide(vector<double> &v, double s){
    //double reciprical = 1.0 / s;
    for(int i=0; i < v.size(); i++){
        v[i] /= (s * 1.0);
    }
}

//for simplicity, we initiate bodies equally spaced on a line
//but add slight random variation to the y-axis
//as to not have y-axis remain 0 for entire program
void initBodies(vector<body> &bodies){
    double spacing = 1.0 / bodies.size();
    for(int i=0; i < bodies.size(); i++){
        double y_offset = rand() * 1.0;
        bodies[i].position[0] = (i * spacing);
        bodies[i].position[1] = (y_offset);
        bodies[i].velocity[0] = 0;
        bodies[i].velocity[1] = 0;
        bodies[i].mass = M;
    }
}

//for debugging purposes
void printBodies(vector<body> bodies){
    for(int i=0; i < bodies.size(); i++){
        cout << "Body " << i << endl;
        cout << "Position(" << bodies[i].position[0] << ", " << bodies[i].position[1] << ") " << endl;
        cout << "Velocity(" << bodies[i].velocity[0] << ", " << bodies[i].velocity[1] << ") " << endl;
        cout << "-------------------" << endl;
    }
}
