#include "physics.h"
#include <iostream>
#include <cmath>
#include <string>
#define M_PI 3.14159265358979323846

template <class T> 
void Simulate<T>::init (T time_step, std::vector< Object<T> > objects, std::vector< Spring<T> > springs, T grav, T drag_force) {

   //Manditory stuff we will always use:
   Simulate<T>::m_objects = objects;
   Simulate<T>::m_dt = time_step;

   //Optional
   Simulate<T>::g = grav;
   Simulate<T>::m_forces_drag = drag_force;
   Simulate<T>::m_springs = springs;
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

   if (m_springs.size() == 0) {
      methods[1] = false;
      std::cout<<"No springs"<<std::endl;
   }
   else {
      methods[1] = true;
   }

   if (m_forces_drag == 0) {
      methods[2] = false;
      std::cout<<"No m_forces_drag"<<std::endl;
   }
   else {
      methods[2] = true;
   }

}

template <class T>
void Simulate<T>::assign_springs () {
   for (int i = 0; i < m_springs.size(); i++) {
      m_springs[i].rest_length = pow(pow(m_objects[m_springs[i].connections[0]].pos[0] - m_objects[m_springs[i].connections[1]].pos[0], 2)+pow(m_objects[m_springs[i].connections[0]].pos[1] - m_objects[m_springs[i].connections[1]].pos[1], 2)+pow(m_objects[m_springs[i].connections[0]].pos[2] - m_objects[m_springs[i].connections[1]].pos[2], 2), 0.5);
   }
}

template <class T>
void Simulate<T>::integrate (int i) {
   //v(t+1) = 1.2 * ^t * (a(t+1) + a(t))
   m_objects[i].vel[0] += 0.5 * m_dt * ((m_objects[i].forces[0]/m_objects[i].m) + m_objects[i].a[0]);
   m_objects[i].vel[1] += 0.5 * m_dt * ((m_objects[i].forces[1]/m_objects[i].m) + m_objects[i].a[1]);
   m_objects[i].vel[2] += 0.5 * m_dt * ((m_objects[i].forces[2]/m_objects[i].m) + m_objects[i].a[2]);

   //x[i](t) = 1/2 * a(t) * ^t**2 + v(t+1) * ^t
   m_objects[i].pos[0] += 0.5 * m_objects[i].a[0]*m_dt*m_dt + m_objects[i].vel[0]*m_dt;
   m_objects[i].pos[1] += 0.5 * m_objects[i].a[1]*m_dt*m_dt + m_objects[i].vel[1]*m_dt;
   m_objects[i].pos[2] += 0.5 * m_objects[i].a[2]*m_dt*m_dt + m_objects[i].vel[2]*m_dt;

   //Setting previous values of accel. for Verlet integrator
   m_objects[i].a[0] = m_objects[i].forces[0]/m_objects[i].m;
   m_objects[i].a[1] = m_objects[i].forces[1]/m_objects[i].m;
   m_objects[i].a[2] = m_objects[i].forces[2]/m_objects[i].m;

   //Reseting Forces
   m_objects[i].forces[0] = 0;
   m_objects[i].forces[1] = 0;
   m_objects[i].forces[2] = 0;
}

template <class T>
void Simulate<T>::forces (int i) {
   //Methods: [grav, springs, drag]
   
   //if gravity: F(t+1)[1] = F(t)[1] - m*g
   if (methods[0]) { 
      m_objects[i].forces[1] -= m_objects[i].m * g;
   }

   //if drag: f(t) = v(t)/drag
   if (methods[2]) {
      m_objects[i].vel[0] /= drag;
      m_objects[i].vel[1] /= drag;
      m_objects[i].vel[2] /= drag;
   }
}

template <class T>
void Simulate<T>::spring_forces (int i) {
   m_springs[i].current_length = pow(pow(m_objects[m_springs[i].connections[0]].pos[0] - m_objects[m_springs[i].connections[1]].pos[0], 2)+pow(m_objects[m_springs[i].connections[0]].pos[1] - m_objects[m_springs[i].connections[1]].pos[1], 2)+pow(m_objects[m_springs[i].connections[0]].pos[2] - m_objects[m_springs[i].connections[1]].pos[2], 2), 0.5);

   if (m_objects[m_springs[i].connections[0]].dynamic) {
      m_objects[m_springs[i].connections[0]].forces[0] -= m_springs[i].k * (m_objects[m_springs[i].connections[0]].pos[0] - m_objects[m_springs[i].connections[1]].pos[0]) * (1 - m_springs[i].rest_length/m_springs[i].current_length);
      m_objects[m_springs[i].connections[0]].forces[1] -= m_springs[i].k * (m_objects[m_springs[i].connections[0]].pos[1] - m_objects[m_springs[i].connections[1]].pos[1]) * (1 - m_springs[i].rest_length/m_springs[i].current_length);
      m_objects[m_springs[i].connections[0]].forces[2] -= m_springs[i].k * (m_objects[m_springs[i].connections[0]].pos[2] - m_objects[m_springs[i].connections[1]].pos[2]) * (1 - m_springs[i].rest_length/m_springs[i].current_length);
   }

   if (m_objects[m_springs[i].connections[1]].dynamic) {
      m_objects[m_springs[i].connections[1]].forces[0] -= m_springs[i].k * (m_objects[m_springs[i].connections[1]].pos[0] - m_objects[m_springs[i].connections[0]].pos[0]) * (1 - m_springs[i].rest_length/m_springs[i].current_length);
      m_objects[m_springs[i].connections[1]].forces[1] -= m_springs[i].k * (m_objects[m_springs[i].connections[1]].pos[1] - m_objects[m_springs[i].connections[0]].pos[1]) * (1 - m_springs[i].rest_length/m_springs[i].current_length);
      m_objects[m_springs[i].connections[1]].forces[2] -= m_springs[i].k * (m_objects[m_springs[i].connections[1]].pos[2] - m_objects[m_springs[i].connections[0]].pos[2]) * (1 - m_springs[i].rest_length/m_springs[i].current_length);
   }
}
