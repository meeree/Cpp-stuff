#include "physics.h"
#include <iostream>
#include <cmath>
#include <string>

template <class T>
Object<T>::Object (T mass, T position[3], T velocity[3], bool dynamic_) {
   Object<T>::m = mass;

   Object<T>::pos[0] = position[0]; 
   Object<T>::pos[1] = position[1];
   Object<T>::pos[2] = position[2];
   Object<T>::pos_0[0] = position[0]; 
   Object<T>::pos_0[1] = position[1];
   Object<T>::pos_0[2] = position[2];

   Object<T>::vel[0] = velocity[0]; 
   Object<T>::vel[1] = velocity[1];
   Object<T>::vel[2] = velocity[2];
   Object<T>::vel_0[0] = velocity[0]; 
   Object<T>::vel_0[1] = velocity[1];
   Object<T>::vel_0[2] = velocity[2];

   Object<T>::a[0] = 0;
   Object<T>::a[1] = 0;
   Object<T>::a[2] = 0;
   Object<T>::forces[0] = 0;
   Object<T>::forces[1] = 0;
   Object<T>::forces[2] = 0;

   Object<T>::dynamic = dynamic_;
}

template <class T> 
void Simulate<T>::init (T time_step, std::vector< Object<T> > objects_, std::vector< Spring<T> > springs_, T grav, T drag_force) {

   //Manditory stuff we will always use:
   Simulate<T>::objects = objects_;
   Simulate<T>::dt = time_step;

   //Optional
   Simulate<T>::g = grav;
   Simulate<T>::drag = drag_force;
   Simulate<T>::springs = springs_;
}

template <class T>
void Simulate<T>::define_methods () {
   if (g == 0) {
      methods[0] = false;
      std::cout<<"No down force"<<std::endl;
   }
   else {
      methods[0] = true;
   }

   if (springs.size() == 0) {
      methods[1] = false;
      std::cout<<"No springs"<<std::endl;
   }
   else {
      methods[1] = true;
   }

   if (drag == 0) {
      methods[2] = false;
      std::cout<<"No drag"<<std::endl;
   }
   else {
      methods[2] = true;
   }

}

template <class T>
void Simulate<T>::assign_springs () {
   for (int i = 0; i < springs.size(); i++) {
      objects[springs[i].connections[0]].bound_to.push_back(springs[i].connections[1]);
      objects[springs[i].connections[1]].bound_to.push_back(springs[i].connections[0]);
      objects[springs[i].connections[0]].bound_by.push_back(i);
      objects[springs[i].connections[1]].bound_by.push_back(i);
   }
}

template <class T>
void Simulate<T>::integrate (int i) {
   //v(t+1) = 1.2 * ^t * (a(t+1) + a(t))
   objects[i].vel[0] += 0.5 * dt * ((objects[i].forces[0]/objects[i].m) + objects[i].a[0]);
   objects[i].vel[1] += 0.5 * dt * ((objects[i].forces[1]/objects[i].m) + objects[i].a[1]);
   objects[i].vel[2] += 0.5 * dt * ((objects[i].forces[2]/objects[i].m) + objects[i].a[2]);

   //x[i](t) = 1/2 * a(t) * ^t**2 + v(t+1) * ^t
   objects[i].pos[0] += 0.5 * objects[i].a[0]*dt*dt + objects[i].vel[0]*dt;
   objects[i].pos[1] += 0.5 * objects[i].a[1]*dt*dt + objects[i].vel[1]*dt;
   objects[i].pos[2] += 0.5 * objects[i].a[2]*dt*dt + objects[i].vel[2]*dt;

   //Setting previous values of accel. for Verlet integrator
   objects[i].a[0] = objects[i].forces[0]/objects[i].m;
   objects[i].a[1] = objects[i].forces[1]/objects[i].m;
   objects[i].a[2] = objects[i].forces[2]/objects[i].m;

   //Reseting Forces
   objects[i].forces[0] = 0;
   objects[i].forces[1] = 0;
   objects[i].forces[2] = 0;
}

template <class T>
void Simulate<T>::forces (int i) {
   //Methods: [grav, springs, drag]
   
   //if gravity: F(t+1)[1] = F(t)[1] - m*g
   if (methods[0]) { 
      objects[i].forces[1] -= objects[i].m * g;
   }

   //if springs: F(t+1) = F(t) - k * ((x_connected(t)-x(t))
   //- (x0_connected(t)-x0(t))
   if (methods[1]) {
      for (int x = 0; x < objects[i].bound_by.size(); x++) {
         objects[i].forces[0] += springs[objects[i].bound_by[x]].k * ((objects[objects[i].bound_to[x]].pos[0] - objects[i].pos[0]) - (objects[objects[i].bound_to[x]].pos_0[0] - objects[i].pos_0[0])/2);
//         objects[i].forces[0] += springs[objects[i].bound_by[x]].c * ((objects[objects[i].bound_to[x]].vel[0] - objects[i].vel[0]) - (objects[objects[i].bound_to[x]].vel_0[0] - objects[i].vel_0[0])/2);

         objects[i].forces[1] += springs[objects[i].bound_by[x]].k * ((objects[objects[i].bound_to[x]].pos[1] - objects[i].pos[1]) - (objects[objects[i].bound_to[x]].pos_0[1] - objects[i].pos_0[1])/2);
//         objects[i].forces[1] += springs[objects[i].bound_by[x]].c * ((objects[objects[i].bound_to[x]].vel[1] - objects[i].vel[1]) - (objects[objects[i].bound_to[x]].vel_0[1] - objects[i].vel_0[1])/2);

         objects[i].forces[2] += springs[objects[i].bound_by[x]].k * ((objects[objects[i].bound_to[x]].pos[2] - objects[i].pos[2]) - (objects[objects[i].bound_to[x]].pos_0[2] - objects[i].pos_0[2])/2);
 //        objects[i].forces[2] += springs[objects[i].bound_by[x]].c * ((objects[objects[i].bound_to[x]].vel[2] - objects[i].vel[2]) - (objects[objects[i].bound_to[x]].vel_0[2] - objects[i].vel_0[2])/2);
      }
   }

   //if drag: f(t) = v(t)/drag
   if (methods[2]) {
      objects[i].vel[0] /= drag;
      objects[i].vel[1] /= drag;
      objects[i].vel[2] /= drag;
   }
}
