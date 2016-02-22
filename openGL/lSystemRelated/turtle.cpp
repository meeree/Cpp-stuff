#include "turtle.h"
#include <iostream>
#include <cmath>

turtle::turtle(std::vector<double> const &position, std::vector<double> const orientation) {
   mPositionStack.push(position);

   double magnitude = 0; /*normalize orientation with magnitude*/
   magnitude += pow(orientation[0], 2);
   magnitude += pow(orientation[1], 2);
   magnitude += pow(orientation[2], 2);
   magnitude = sqrt(magnitude);
   mOrientationStack.push({0, 0, 0});
   for (int iComponent = 0; iComponent < 3; iComponent++) {
      mOrientationStack.top()[iComponent] = orientation[iComponent] / magnitude;
   }
}

std::vector<double> turtle::Rx(double const &theta) {
   std::vector<double> const &curOri = mOrientationStack.top();
   mOrientationStack.push({1, 0, 0});
   mOrientationStack.top()[1] = curOri[1] * cos(theta) - curOri[2] * sin(theta);
   mOrientationStack.top()[2] = curOri[1] * sin(theta) + curOri[2] * cos(theta);
   return mOrientationStack.top();
}

std::vector<double> turtle::Ry(double const &theta) {
   std::vector<double> const &curOri = mOrientationStack.top();
   mOrientationStack.push({0, 1, 0});
   mOrientationStack.top()[0] = curOri[0] * cos(theta) + curOri[2] * sin(theta);
   mOrientationStack.top()[2] = -curOri[0] * sin(theta) + curOri[2] * cos(theta);
   return mOrientationStack.top();
}

std::vector<double> turtle::Rz(double const &theta) {
   std::vector<double> const &curOri = mOrientationStack.top();
   mOrientationStack.push({0, 0, 1});
   mOrientationStack.top()[0] = curOri[0] * cos(theta) - curOri[1] * sin(theta);
   mOrientationStack.top()[1] = curOri[0] * sin(theta) + curOri[1] * cos(theta);
   return mOrientationStack.top();
}
   
std::vector<double> turtle::Mv(double const &distance) {
   mPositionStack.push(mPositionStack.top());
   for (int iComponent = 0; iComponent < 3; iComponent++) {
      mPositionStack.top()[iComponent] += distance * mOrientationStack.top()[iComponent];
   }
   return mPositionStack.top();
}

int main () {
   turtle toitle({0, 0, 0}, {0, 1, 0});
}
