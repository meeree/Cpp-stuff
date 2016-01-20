#include<vector>
#ifndef PHYSICS_H
#define PHYSICS_H

template <class T>
class Object {
   public:
   T m;
   T pos[3], vel[3], pos_0[3], vel_0[3], a[3], forces[3];
   bool dynamic;
   std::vector<int> bound_to, bound_by;
   Object(T mass, T position[3], T velocity[3], bool dynamic_);
};

template <class T>
struct Spring {
   T k;
   T c;
   int connections[2]; 
};

template <class T> 
class Simulate {
   private:
   T g;
   T dt;
   T drag;
   bool methods[3] {};
   public:
   std::vector< Object<T> > objects;
   std::vector< Spring<T> > springs;

   void init(T time_step, std::vector< Object<T> > objects_, std::vector< Spring<T> > springs_, T grav, T drag_force);

   void integrate(int i);
   void forces(int i);
   void define_methods();
   void assign_springs();
};

#endif
