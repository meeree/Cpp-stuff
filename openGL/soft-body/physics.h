#include<vector>
#ifndef PHYSICS_H
#define PHYSICS_H

class jEng {
public:
   jEng (double const &cTimeStep, std::vector<object> const &cObjects);
   void toggleDownGravity (double const &cDownGrav);   
   void toggleSprings (std::vector<spring> &rSprings);
   void toggleDrag (double const &cDragForce);
   void addObject (object &newObject);
   void addSpring (spring &newSpring);
private:
   double mTimeStep;
   std::vector<object> mObjects;
   bool mForceMethods [3];
   double mDownGrav;
   std::vector<spring> mSprings;
   double mDragForce;
};

template <class T>
struct object {
   public:
   T m;
   T pos[3], vel[3], a[3], forces[3];
   bool dynamic;
};

template <class T>
struct spring {
   T k, c;
   int connections[2]; 
   T current_length, rest_length;
};

#endif
