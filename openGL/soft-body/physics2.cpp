
//spring class>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
spring::spring (std::vector<unsigned> const &cConnection, double const &cHookesConst) { 
   spring::mConnection = cConnection;
   spring::mHookesConst = cHookesConst;
   spring::mRestLength = 0;
}

void spring::updateRestLength () {
   double const &cDeltaX = mConnection[1] - Connection[0];
   double const &cDeltaY = mConnection[1] - Connection[0];
   double const &cDeltaZ = mConnection[1] - Connection[0];
   spring::mRestLength = sqrt(cDeltaX*cDeltaX + cDeltaY*cDeltaY + cDeltaZ*cDeltaZ);
}

void spring::addConnection (unsigned const &cIndex) {
   spring::mConnection.push_back(cIndex);
   assert(spring::mConnection < 3);
}

void spring::deleteConnection (unsigned const &cIndex) {
   spring::mConnection.erase(spring.begin() + cIndex);
   assert(spring::mConnection > 0);
}
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

//jEngine class>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
jEng::jEng (double const &cTimeStep, std::vector<pointMass> const &pointMasses) {
   jEng::mTimeStep = cTimeStep;
   jEng::mPointMasses = cPointMasses;
   jEng::mForces = {false, false, false}
}

void jEng::setDownGravity (double const &cDownGrav) {
   jEng::mDownGrav = cDownGrav;

   if (cDownGrav == 0) {
      mForces[0] = false;
      std::cout<<"Removed down gravity"<<std::endl;
   } else {
      mForces[0] = true;
      std::cout<<"Added down gravity"<<std::endl;
   }
}

void jEng::setSprings (std::vector<spring> const &cSprings) {
   for (auto &iSpring: cSprings) {
      if (iSpring.connection.size() > 1) {
         iSpring.updateRestLength();
      }
   }

   jEng::mSprings = cSprings;

   if (cSprings.size() == 0) {
      mForces[1] = false;
      std::cout<<"Removed springs"<<std::endl;
   } else {
      mForces[1] = true;
      std::cout<<"Added springs"<<std::endl;
   }
}

void jEng::setDrag (double dragForce) {
   jEng::mDragForce = dragForce;
   if (dragForce == 0) {
      mForces[2] = false;
      std::cout<<"Removed drag force"<<std::endl;
   } else {
      mForces[2] = true;
      std::cout<<"Added drag force"<<std::endl;
   }
}

void jEng::update () {
   for (auto &iPointMass: mPointMasses) {
      //ADD OBJECT VERLET MEMBER FUNCTION
      //ADD OBJECT FORCES 
      iPointMass.forces();
      iPointMass.integrate();
      iPointMass.updateAccelaration();
   }
}

void jEng::addPointMass (pointMass &rPointMass) {
   rPointMass.forces();
   rPointMass.updateAccelaration();
}
