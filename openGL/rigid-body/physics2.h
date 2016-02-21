class object {
private:
   friend class particle;
   std::vector<particle> mParticles;
   double mMass;
   double mInertiaInv[3][3];
public:
   object(double const &mass, double const (&inertiaInv)[3][3], std::vector<particle> const &particles={});
   void updateForces();
   void applyForcesAndTorques();
   //move will take points and update it with new particle positions
   void move(double const &dt, std::vector<double> &points);
};

class particle {
private:
   std::vector<force> mForceMethods;
   double mPos[3], mVel[3], mRot[3][3], mOmega[3], mTorque[3], mForce[3], mDis[3];
public:
   particle(double const (&pos)[3], double const (&dis)[3], double const (&vel)[3]={0,0,0});
};
   
class force {
private:
   friend class particle;
protected:
   std::vector<double> mConstants;
   std::vector<particle> mParticles;
   force(std::vector<double> const &constants,std::vector<particle> const &particles);
   virtual void update() {};
   virtual void applyForce() = 0;
};
