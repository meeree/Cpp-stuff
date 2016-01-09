#include <iostream>
#include <math.h>
#include <vector>
#include <fstream>
#include <ctime>
#include <string>

static struct {
   int n = 20;
   std::ofstream fl;
   float ke[3];
} info;

struct Object {
   float m;
   std::vector<double> pos;
   std::vector<double> vel;
   bool dynamic;
   std::vector<double> forces;
   std::vector<double> a;
};

struct Spring {
   double k;
   double c;
   int connections[2];
   double rest_length;
};

class Simulate {
   public:
      double g;
      float dt;
      std::vector<Object> objects;
      std::vector<Spring> springs;
         
      void set_rest_length() {
         for (int i = 0; i < springs.size(); i++) {
            springs[i].rest_length = pow(pow(objects[springs[i].connections[0]].pos[0]-objects[springs[i].connections[1]].pos[0], 2)+pow(objects[springs[i].connections[0]].pos[1]-objects[springs[i].connections[1]].pos[1], 2)+pow(objects[springs[i].connections[0]].pos[2]-objects[springs[i].connections[1]].pos[2], 2), 0.5);
         }
      }

      void integrate() {
         for (int i=0; i < objects.size(); i++) {
            if (objects[i].dynamic) {
               info.ke[0] += 0.5 * objects[i].m * (0.5 * dt * ((objects[i].forces[0]/objects[i].m) + objects[i].a[0]) * 0.5 * dt * ((objects[i].forces[0]/objects[i].m) + objects[i].a[0]) - objects[i].vel[0] * objects[i].vel[0]);
               info.ke[1] += 0.5 * objects[i].m * (0.5 * dt * ((objects[i].forces[1]/objects[i].m) + objects[i].a[1]) * 0.5 * dt * ((objects[i].forces[1]/objects[i].m) + objects[i].a[1]) - objects[i].vel[1] * objects[i].vel[1]);
               info.ke[2] += 0.5 * objects[i].m * (0.5 * dt * ((objects[i].forces[2]/objects[i].m) + objects[i].a[2]) * 0.5 * dt * ((objects[i].forces[2]/objects[i].m) + objects[i].a[2]) - objects[i].vel[2] * objects[i].vel[2]);
               objects[i].vel[0] += 0.5 * dt * ((objects[i].forces[0]/objects[i].m) + objects[i].a[0]);
               objects[i].vel[1] += 0.5 * dt * ((objects[i].forces[1]/objects[i].m) + objects[i].a[1]);
               objects[i].vel[2] += 0.5 * dt * ((objects[i].forces[2]/objects[i].m) + objects[i].a[2]);
               objects[i].pos[0] += 0.5 * objects[i].a[0]*dt*dt + objects[i].vel[0]*dt;
               objects[i].pos[1] += 0.5 * objects[i].a[1]*dt*dt + objects[i].vel[1]*dt;
               objects[i].pos[2] += 0.5 * objects[i].a[2]*dt*dt + objects[i].vel[2]*dt;
               objects[i].forces[0] = 0; objects[i].forces[1] = 0; objects[i].forces[2] = 0;
               objects[i].a[0] = objects[i].forces[0]/objects[i].m; objects[i].a[1] = objects[i].forces[1]/objects[i].m; objects[i].a[2] = objects[i].forces[2]/objects[i].m;
            }       
         }
         info.fl<<info.ke[0]<<","<<info.ke[1]<<","<<info.ke[2]<<"\n";
      }

      void forces() {
         for (int i=0; i < objects.size(); i++) {
            if (objects[i].dynamic) {
               objects[i].forces[1] -= objects[i].m * g;
            }
         }

         for (int i = 0; i < springs.size(); i++) {
            if ((objects[springs[i].connections[0]].dynamic) or (objects[springs[i].connections[1]].dynamic)) {
               double cur_length = pow(pow(objects[springs[i].connections[0]].pos[0]-objects[springs[i].connections[1]].pos[0], 2)+pow(objects[springs[i].connections[0]].pos[1]-objects[springs[i].connections[1]].pos[1], 2)+pow(objects[springs[i].connections[0]].pos[2]-objects[springs[i].connections[1]].pos[2], 2), 0.5);

               double norm[3];
               norm[0] = (objects[springs[i].connections[0]].pos[0]-objects[springs[i].connections[1]].pos[0])/cur_length;
               norm[1] = (objects[springs[i].connections[0]].pos[1]-objects[springs[i].connections[1]].pos[1])/cur_length;
               norm[2] = (objects[springs[i].connections[0]].pos[2]-objects[springs[i].connections[1]].pos[2])/cur_length;

//               if (cur_length > 1.1*springs[i].rest_length) {
//                  objects.push_back(objects[springs[i].connections[1]]);
//                  springs[i].connections[1] = objects.size()-1;
//                  cur_length = pow(pow(objects[springs[i].connections[0]].pos[0]-objects[springs[i].connections[1]].pos[0], 2)+pow(objects[springs[i].connections[0]].pos[1]-objects[springs[i].connections[1]].pos[1], 2)+pow(objects[springs[i].connections[0]].pos[2]-objects[springs[i].connections[1]].pos[2], 2), 0.5);
//
//                  norm[0] = (objects[springs[i].connections[0]].pos[0]-objects[springs[i].connections[1]].pos[0])/cur_length;
//                  norm[1] = (objects[springs[i].connections[0]].pos[1]-objects[springs[i].connections[1]].pos[1])/cur_length;
//                  norm[2] = (objects[springs[i].connections[0]].pos[2]-objects[springs[i].connections[1]].pos[2])/cur_length;
//               }

               double dot = (objects[springs[i].connections[0]].vel[0]-objects[springs[i].connections[1]].vel[0]) * norm[0] + (objects[springs[i].connections[0]].vel[1]-objects[springs[i].connections[1]].vel[1]) * norm[1] + (objects[springs[i].connections[0]].vel[2]-objects[springs[i].connections[1]].vel[2]) * norm[2];

               if (objects[springs[i].connections[0]].dynamic) {
                  objects[springs[i].connections[0]].forces[0] -= (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[0];
                  objects[springs[i].connections[0]].forces[1] -= (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[1];
                  objects[springs[i].connections[0]].forces[2] -= (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[2];
               }

               if (objects[springs[i].connections[1]].dynamic) {
                  objects[springs[i].connections[1]].forces[0] += (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[0];
                  objects[springs[i].connections[1]].forces[1] += (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[1];
                  objects[springs[i].connections[1]].forces[2] += (springs[i].k * (cur_length-springs[i].rest_length) + springs[i].c * dot) * norm[2];
               }
            }
         }
      }
};


void cloth (int n, float struc_k, float shear_k, float bend_k, float c, float m, float dis, std::string mode, std::vector<Object>& objects, std::vector<Spring>& springs) {
   if (mode == "square") {
      for (int j = 0; j < n+1; j++) {
         for (int i = 0; i < n+1; i++) {
            if ((i == n) or (j == n) or (i == 0) or (j == 0)) {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, false, {0, 0, 0}, {0, 0, 0}});
            }
            else {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, true, {0, 0, 0}, {0, 0, 0}});
            }

            if (i < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+1+j*(n+1)}, 0});
            }
            if (j < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+(j+1)*(n+1)}, 0});
            }
            if (i < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+2+j*(n+1)}, 0});
            }
            if (j < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+(j+2)*(n+1)}, 0});
            }
            if ((0 < i) and (i < n) and (0 < j) and (j < n)) {
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j-1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j-1)*(n+1)}, 0});
            }
         }
      }
   }
   if (mode == "4 edges") {
      for (int j = 0; j < n+1; j++) {
         for (int i = 0; i < n+1; i++) {
            if (((i == n) and (j == n)) or ((i == n) and (j == 0)) or ((i == 0) and (j == n)) or ((i == 0) and (j == 0))) {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, false, {0, 0, 0}, {0, 0, 0}});
            }
            else {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, true, {0, 0, 0}, {0, 0, 0}});
            }
            if (i < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+1+j*(n+1)}, 0});
            }
            if (j < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+(j+1)*(n+1)}, 0});
            }
            if (i < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+2+j*(n+1)}, 0});
            }
            if (j < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+(j+2)*(n+1)}, 0});
            }
            if ((0 < i) and (i < n) and (0 < j) and (j < n)) {
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j-1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j-1)*(n+1)}, 0});
            }
         }
      }
   }
   if (mode == "1 edge") {
      for (int j = 0; j < n+1; j++) {
         for (int i = 0; i < n+1; i++) {
            if ((i == n) and (j == n)) {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, false, {0, 0, 0}, {0, 0, 0}});
            }
            else {
               objects.push_back(Object {m, {dis * (i-n/2), 0, dis * (j-n/2)}, {0, 0, 0}, true, {0, 0, 0}, {0, 0, 0}});
            }

            if (i < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+1+j*(n+1)}, 0});
            }
            if (j < n) {
               springs.push_back(Spring {struc_k, c, {i+j*(n+1), i+(j+1)*(n+1)}, 0});
            }
            if (i < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+2+j*(n+1)}, 0});
            }
            if (j < n-1) {
               springs.push_back(Spring {bend_k, c, {i+j*(n+1), i+(j+2)*(n+1)}, 0});
            }
            if ((0 < i) and (i < n) and (0 < j) and (j < n)) {
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i+1+(j-1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j+1)*(n+1)}, 0});
               springs.push_back(Spring {shear_k, c, {i+j*(n+1), i-1+(j-1)*(n+1)}, 0});
            }
         }
      }
   }
}

int main () {
   std::vector<Object> objects;
   std::vector<Spring> springs;
   Simulate sim {0.00001, 0.7, objects, springs};
   info.fl.open("fl.csv", std::ios::out);
   info.fl.clear();
   cloth (info.n, 0.01, 0.01, 0.01, 0.3, 1, 0.04, "4 edges", sim.objects, sim.springs);
   sim.set_rest_length();
   std::time_t t = std::time(NULL);
   int frame = 0;
   while (true) {
      sim.forces();
      sim.integrate();
      frame++;
      if (std::time(NULL)-t >= 1) {
         std::cout<<frame<<std::endl;
         t = std::time(NULL);
         frame = 0;
      }
   }
}
