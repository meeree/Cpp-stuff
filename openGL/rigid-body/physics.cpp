#include <iostream>
#include <vector>
#include <cmath>

//particle struct>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 

struct particle {
   double mPos[3], mDis[3], mVel[3], mOmega[3], mForce[3], mTorque[3];
   bool mDynamic;
   double mMagDis;
   double mRot[3][3];
};

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//object struct>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

struct object {  
   double mMass;
   double mIinv[3][3];
   std::vector<particle*> mParticles;
   bool mDynamic;
   double mPos[3], mVel[3], mForce[3];
   virtual void applyTorque () {

   };
};

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//object >> pointMass>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

struct pointMass : public object {
   pointMass (double const &mass, double const (&pos)[3], particle* const &particle, bool const &dynamic=true, double const (&force)[3]={0,0,0}) {
      mMass = mass;
      mDynamic = dynamic;
      for (unsigned iComp=0; iComp < 3; iComp++) {
         particle->mPos[iComp] = pos[iComp];
         particle->mDis[iComp] = 0;
         mPos[iComp] = pos[iComp];
         mVel[iComp] = 0;
         mForce[iComp] = force[iComp];
      }
      particle->mDynamic = dynamic;
      mParticles = {particle};
   }
};

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//force class>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

struct force {
   virtual void applyForce() = 0;
   virtual void update() {};
};

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//Inhertied from force>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//force >> drag>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

struct drag : public force {
   drag (object* objectOne, double const &kDrag) {
      mObjectOne = objectOne;
      mKdrag = kDrag;
   }

   void applyForce () {
      //F -= vD
      for (unsigned iComp=0; iComp < 3; iComp++) {
         mObjectOne->mForce[iComp] -= mObjectOne->mVel[iComp] * mKdrag;
      }
   }

private:
   double mKdrag;
   object* mObjectOne;
};

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//force >> gravity>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

struct gravity : public force {
   gravity (object* objectOne, double const &kGrav) {
      mKgrav = kGrav;
      mObjectOne = objectOne;
   }

   void applyForce () {
      //F -= mG
      mObjectOne->mForce[1] -= mObjectOne->mMass * mKgrav;
   }

private:
   double mKgrav;
   object* mObjectOne;
};

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//force >> spring>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

struct spring : public force {
   spring (particle* particleOne, particle* particleTwo, double const &kSpring, double const &kDamp) {

      mKspring = kSpring;
      mKdamp = kDamp;
      mParticleOne = particleOne;
      mParticleTwo = particleTwo;

      //Setting deltaX0 and rest length
      mRestLength = 0;
      for (unsigned iComp=0; iComp < 3; iComp++) {
         mRestLength += pow(particleOne->mPos[iComp] - particleTwo->mPos[iComp], 2);
         mDeltaX0[iComp] = particleOne->mPos[iComp] - particleTwo->mPos[iComp];
      }
      mRestLength = sqrt(mRestLength);
      mRestLength = 0;
   }

   void update () {
      //Setting deltaX, deltaV, and current length
      mCurLength = 0; mDot = 0;
      for (unsigned iComp=0; iComp < 3; iComp++) {
         mCurLength += pow(mParticleOne->mPos[iComp] - mParticleTwo->mPos[iComp], 2);
         mDeltaX[iComp] = mParticleOne->mPos[iComp] - mParticleTwo->mPos[iComp];
         mDeltaV[iComp] = mParticleOne->mVel[iComp] - mParticleTwo->mVel[iComp];
         mDot += mDeltaV[iComp] * mDeltaX[iComp];
      }
      mCurLength = sqrt(mCurLength);
   }

   void applyForce () {
      //F = -[ks(mag(deltaX) - mag(deltaX0)) + kd((deltaX. deltaV)/mag(deltaX))] * deltaX/mag(deltaX)
      for (unsigned iComp=0; iComp < 3; iComp++) {
         double const &force = (mKspring * (mCurLength - mRestLength) + mKdamp * (mDot / mCurLength)) * mDeltaX[iComp] / mCurLength;
         if (mParticleOne->mDynamic) {
            mParticleOne->mForce[iComp] -= force;
         }
         if (mParticleTwo->mDynamic) {
            mParticleTwo->mForce[iComp] += force;
         }
      }
   }

private:
   double mDeltaX0[3], mDeltaX[3], mDeltaV[3];
   double mRestLength, mCurLength, mDot, mKdamp, mKspring;
   particle* mParticleOne;
   particle* mParticleTwo;
};


//jEngine class>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 

class jEng {
public:
   jEng (std::vector<object*> const &objects, std::vector<force*> const &forces, double const &dt) {
      mForces = forces;
      mObjects = objects;
      mDt = dt;
      std::cout<<"_____________/\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\___________________________________________________________v0.0                \n_______/\\\\\\_\\/\\\\\\///////////________________________________________________________________              \n______\\///__\\/\\\\\\_____________________________/\\\\\\\\\\\\\\\\___/\\\\\\______________________________          \n_______/\\\\\\_\\/\\\\\\\\\\\\\\\\\\\\\\______/\\\\/\\\\\\\\\\\\____/\\\\\\////\\\\\\_\\///___/\\\\/\\\\\\\\\\\\_______/\\\\\\\\\\\\\\\\__        \n______\\/\\\\\\_\\/\\\\\\///////______\\/\\\\\\////\\\\\\__\\//\\\\\\\\\\\\\\\\\\__/\\\\\\_\\/\\\\\\////\\\\\\____/\\\\\\/////\\\\\\_        \n______\\/\\\\\\_\\/\\\\\\_____________\\/\\\\\\__\\//\\\\\\__\\///////\\\\\\_\\/\\\\\\_\\/\\\\\\__\\//\\\\\\__/\\\\\\\\\\\\\\\\\\\\\\__      \n__/\\\\_\\/\\\\\\_\\/\\\\\\_____________\\/\\\\\\___\\/\\\\\\__/\\\\_____\\\\\\_\\/\\\\\\_\\/\\\\\\___\\/\\\\\\_\\//\\\\///////___    \n_\\//\\\\\\\\\\\\__\\/\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\_\\/\\\\\\___\\/\\\\\\_\\//\\\\\\\\\\\\\\\\__\\/\\\\\\_\\/\\\\\\___\\/\\\\\\__\\//\\\\\\\\\\\\\\\\\\\\_\n__\\//////___\\///////////////__\\///____\\///___\\////////___\\///__\\///____\\///____\\/////////  "<<std::endl;
   }

   void updateForcesAndTorques() {
      //Apply forces and update member data in force if necessary
      for (auto &iForce: mForces) {
         iForce->update();
         iForce->applyForce();
      }
      //Apply torques using objects defined torque method or do nothing if no method
      for (auto &iObj: mObjects) {
         iObj->applyTorque();
      }
   }

   void integrate() {
      //Update position and velocity and reset forces
      for (auto &iObj: mObjects) {
         if (iObj->mDynamic) {
            for (unsigned iComp=0; iComp < 3; iComp++) {
               iObj->mVel[iComp] += (iObj->mForce[iComp] / iObj->mMass) * mDt;
               iObj->mPos[iComp] += iObj->mVel[iComp] * mDt;
               for (auto &iPart: iObj->mParticles) {
                     iPart->mVel[iComp] += iObj->mForce[iComp] + (iPart->mForce[iComp] / iObj->mMass) * mDt;
                     iPart->mPos[iComp] += iPart->mVel[iComp] * mDt;
                     iPart->mForce[iComp] = 0;
               }
               iObj->mForce[iComp] = 0;
            }
         }
      }
   }


   void update() {
      updateForcesAndTorques();
      integrate();
      std::cout<<mObjects[0]->mPos[1]<<std::endl;
   }

private:
   std::vector<object*> mObjects;
   double mDt;
   std::vector<force*> mForces; 
};

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

int main () {
   double pos[3] = {0, 0, 0};
   double mass = 1;
   particle part;
   pointMass p (mass, pos, &part);
   gravity grav (&p, 9.8);
   jEng jamie ({&p}, {&grav}, 0.1);
   for (unsigned i=0; i < 10; i++) {
      jamie.update();
   }
}
