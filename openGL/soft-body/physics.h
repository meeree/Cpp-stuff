#include<vector>
#ifndef PHYSICS_H
#define PHYSICS_H

template <class T>
struct Object {
   public:
   T m;
   T pos[3], vel[3], a[3], forces[3];
   bool dynamic;
};

template <class T>
struct Spring {
   T k, c;
   int connections[2]; 
   T current_length, rest_length;
};

template <class T> 
class Simulate {
   private:
   T m_forces_g;
   T m_forces_drag;
   T m_dt;
   bool m_methods[3] {};
   public:
   std::vector< Object<T> > m_objects;
   std::vector< Spring<T> > m_springs;

   void init(T time_step, std::vector< Object<T> > objects, std::vector< Spring<T> > springs, T grav, T drag_force);

   void integrate(int i);
   void forces(int i);
   void spring_forces(int i);
   void define_methods();
   void assign_springs();
};

#endif
