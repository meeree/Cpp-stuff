#ifndef stack
#include <stack>
#endif 
#ifndef vector
#include <vector>
#endif 

class turtle {
public:
   turtle(std::vector<double> const &position, std::vector<double> const orientation);
   std::vector<double> Rx(double const &theta);
   std::vector<double> Ry(double const &theta);
   std::vector<double> Rz(double const &theta);
   std::vector<double> Mv(double const &distance);
private:
   std::stack<std::vector<double> > mOrientationStack, mPositionStack;
};
