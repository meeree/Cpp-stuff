#include <iostream>
#include <vector>
#include <cmath>

//particle struct>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 

struct particle {
   double mPos[3], mDis[3], mDis0[3], mOmega[3], mForce[3], mTorque[3];
   bool mDynamic;
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
      for (auto &iPart: mParticles) {
         iPart->mTorque[0] = iPart->mForce[1] * iPart->mDis[2] - iPart->mForce[2] * iPart->mDis[1];
         iPart->mTorque[1] = iPart->mForce[2] * iPart->mDis[0] - iPart->mForce[0] * iPart->mDis[2];
         iPart->mTorque[2] = iPart->mForce[0] * iPart->mDis[1] - iPart->mForce[1] * iPart->mDis[0];
         std::cout<<iPart->mTorque[0]<<","<<iPart->mTorque[1]<<","<<iPart->mTorque[2]<<std::endl<<std::endl;
      }
   }
};

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//object >> pointMass>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

struct pointMass : public object {
   pointMass (double const &mass, double const (&pos)[3], particle* const &particle, bool const &dynamic=true, double const (&force)[3]={0,0,0}) {
      mMass = mass;
      mDynamic = dynamic;
      for (unsigned iComp=0; iComp < 3; iComp++) {
         particle->mPos[iComp] = pos[iComp];
         particle->mDis0[iComp] = 0;
         particle->mDis[iComp] = 0;
         mPos[iComp] = pos[iComp];
         mForce[iComp] = force[iComp];
      }

      mDynamic = dynamic;
      mParticles = {particle};
   }
   void applyTorque () {};
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

//force >> applied>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

struct applied : public force {
   applied (particle* particleOne, double const &kApp, std::vector<unsigned> const &dims) {
      mKapp = kApp;
      mParticleOne = particleOne;
      mDims = dims;
   }

   void applyForce () {
      for (auto const &dim: mDims) {
         mParticleOne->mForce[dim] += mKapp;
      }
   }

private:
   double mKapp;
   particle* mParticleOne;
   std::vector<unsigned> mDims;
};

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//force >> spring>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

struct spring : public force {
   spring (object* objectOne, unsigned const &iPartOne, object* objectTwo, unsigned const &iPartTwo, double const &kSpring, double const &kDamp) {

      mKspring = kSpring;
      mKdamp = kDamp;
      mObjectOne = objectOne;
      mObjectTwo = objectTwo;

      mParticleOne = objectOne->mParticles[iPartOne];
      mParticleTwo = objectTwo->mParticles[iPartTwo];

      //Setting deltaX0 and rest length
      mRestLength = 0;
      for (unsigned iComp=0; iComp < 3; iComp++) {
         mRestLength += pow(mParticleOne->mPos[iComp] - mParticleTwo->mPos[iComp], 2);
         mDeltaX0[iComp] = mParticleOne->mPos[iComp] - mParticleTwo->mPos[iComp];
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
         mDeltaV[iComp] = mObjectOne->mVel[iComp] - mObjectTwo->mVel[iComp];
         mDot += mDeltaV[iComp] * mDeltaX[iComp];
      }
      mCurLength = sqrt(mCurLength);
   }

   void applyForce () {
      //F = -[ks(mag(deltaX) - mag(deltaX0)) + kd((deltaX. deltaV)/mag(deltaX))] * deltaX/mag(deltaX)
      for (unsigned iComp=0; iComp < 3; iComp++) {
         double const &force = (mKspring * (mCurLength - mRestLength) + mKdamp * (mDot / mCurLength)) * mDeltaX[iComp] / mCurLength;
         if (mObjectOne->mDynamic) {
            mObjectOne->mForce[iComp] -= force;
            mParticleOne->mForce[iComp] -= force;
         }
         if (mObjectTwo->mDynamic) {
            mObjectTwo->mForce[iComp] += force;
            mParticleTwo->mForce[iComp] += force;
         }
      }
   }

private:
   double mDeltaX0[3], mDeltaX[3], mDeltaV[3];
   double mRestLength, mCurLength, mDot, mKdamp, mKspring;
   object* mObjectOne;
   object* mObjectTwo;
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
               //translational on object
               iObj->mVel[iComp] += (iObj->mForce[iComp] / iObj->mMass) * mDt;
               iObj->mPos[iComp] += iObj->mVel[iComp] * mDt;
               for (auto &iPart: iObj->mParticles) {
                  //rotational on particles
                  iPart->mOmega[iComp] += iPart->mTorque[iComp] * mDt;
                  iPart->mOmega[iComp] = iPart->mOmega[0]*iObj->mIinv[iComp][0] + iPart->mOmega[1]*iObj->mIinv[iComp][1] + iPart->mOmega[2]*iObj->mIinv[iComp][2];
                  iPart->mRot[0][iComp] += mDt * (-iPart->mOmega[2] * iPart->mRot[1][iComp] + iPart->mOmega[1] * iPart->mRot[2][iComp]);
                  iPart->mRot[1][iComp] += mDt * (iPart->mOmega[2] * iPart->mRot[0][iComp] - iPart->mOmega[0] * iPart->mRot[2][iComp]);
                  iPart->mRot[2][iComp] += mDt * (-iPart->mOmega[1] * iPart->mRot[0][iComp] + iPart->mOmega[0] * iPart->mRot[1][iComp]);

                  //translational on particles
                  iPart->mDis[iComp] = iPart->mRot[iComp][0] * iPart->mDis0[0] + iPart->mRot[iComp][1] * iPart->mDis0[1] + iPart->mRot[iComp][2] * iPart->mDis0[2];
                  iPart->mPos[iComp] = iObj->mPos[iComp] + iPart->mDis[iComp];
                  iPart->mForce[iComp] = 0;
                  std::cout<<iPart->mPos[iComp]<<std::endl;
               }
               iObj->mForce[iComp] = 0;
            }
         }
      }
   }


   void update() {
      updateForcesAndTorques();
      integrate();
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
   applied app (&part, 5, {1});
   jEng jamie ({&p}, {&grav, &app}, 0.1);
   for (unsigned i=0; i < 10; i++) {
      jamie.update();
   }
}
