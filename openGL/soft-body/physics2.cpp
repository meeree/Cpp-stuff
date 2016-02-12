
//jEngine class>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
jEng::jEng (double const &cTimeStep, std::vector<pointMass> const &pointMasses) {
   jEng::mTimeStep = cTimeStep;
   jEng::mPointMasses = cPointMasses;
   jEng::mForceMethods = {false, false, false};
}

void jEng::setDownGravity (double const &cDownGrav) {
   jEng::mDownGrav = cDownGrav;

   if (cDownGrav == 0) {
      mForceMethods[0] = false;
      std::cout<<"Removed down gravity"<<std::endl;
   } else {
      mForceMethods[0] = true;
      std::cout<<"Added down gravity"<<std::endl;
   }
}

void jEng::setSprings (std::vector<spring> &rSprings) {
   for (auto &iSpring: rSprings) {
      if (iSpring.connection.size() > 1) {
         pointMass const &cPointMassOne = mPointMasses[iSpring.connection[0]] 
         pointMass const &cPointMassTwo = mPointMasses[iSpring.connection[1]] 
         double const &cDeltaX = cPointMassTwo

         spring::mRestLength = sqrt(cDeltaX*cDeltaX + cDeltaY*cDeltaY + cDeltaZ*cDeltaZ);
      }
   }

   jEng::mSprings = cSprings;

   if (cSprings.size() == 0) {
      mForceMethods[1] = false;
      std::cout<<"Removed springs"<<std::endl;
   } else {
      mForceMethods[1] = true;
      std::cout<<"Added springs"<<std::endl;
   }
}

void jEng::setDrag (double dragForce) {
   jEng::mDragForce = dragForce;
   if (dragForce == 0) {
      mForceMethods[2] = false;
      std::cout<<"Removed drag force"<<std::endl;
   } else {
      mForceMethods[2] = true;
      std::cout<<"Added drag force"<<std::endl;
   }
}

void jEng::update () {
   for (auto &iPointMass: mPointMasses) {
      //ADD OBJECT VERLET MEMBER FUNCTION
      //ADD OBJECT FORCES 
      iPointMass.forces();
      iPointMass.integrate();
      iPointMass.updateAcceleration();
   }

   if (mForceMethods[1]) {
      for (auto const &iSpring: mSprings) {
         pointMass &pointMassOne = mPointMasses[iSpring.
         iSpring.forces();
      }
   }
}

void jEng::addPointMass (pointMass &rPointMass) {
   rPointMass.forces();
   rPointMass.updateAcceleration();
   pointMass newPointMass = rPointMass;
   mPointMasses.push_back(newPointMass);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//pointMass class>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
pointMass::pointMass (double const &cMass, std::vector<double> const &cPosition, double const &cRadius = 0, std::vector<double> const &cVelocity = {0, 0, 0}) {
   pointMass::mMass = cMass;
   pointMass::mPosition = cPosition;

   //these default to zero 
   pointMass::mRadius = cRadius;
   pointMass::mVelocity = cVelocity;
   pointMass::mAcceleration = {0, 0, 0};
   pointMasss::mForce = {0, 0, 0};
}

pointMass::forces () {
} 


//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//spring class>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
spring::spring (std::vector<unsigned> const &cConnection, double const &cHookesConst) { 
   spring::mConnection = cConnection;
   spring::mHookesConst = cHookesConst;
   spring::mRestLength = 0;
}

void spring::updateRestLength () {
}

void spring::addConnection (unsigned const &cIndex) {
   spring::mConnection.push_back(cIndex);
   assert(spring::mConnection < 3);
}

void spring::deleteConnection (unsigned const &cIndex) {
   spring::mConnection.erase(spring.begin() + cIndex);
   assert(spring::mConnection > 0);
}

unsigned spring::getConnection (unsigned const &cIndex) {
   assert(cIndex+1 > mConnection.size());
   return mConnection[cIndex];
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

