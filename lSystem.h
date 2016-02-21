#ifndef vector
#include <vector>
#endif
#ifndef string
#include <string>
#endif
#ifndef functional
#include <functional>
#endif
#ifndef map
#include <map>
#endif

typedef std::function<bool (double)> condition; /* condition to be satisfied eg: t > 3 */
typedef std::function<double (double)> mapping; /*maps a parameter eg: t->t/3*/
//typedef std::function<void (turtle)> turtleCall; /*calls the specified function in the turtle class eg: turtle.Mv*/

class symbol {
public:
   symbol(std::vector<double> const &params, std::vector<condition> const &conditions, std::vector<mapping> const &mappings);
   void map();

private:
   std::vector<double> mParams;
   std::vector<condition> mConditions;
   std::vector<mapping> mMappings;
};

class lSystem {
public:
   lSystem(std::vector<symbol> const &axiom, std::vector<symbol> * endString, std::vector<symbol> const &variables, std::vector<symbol> const &constants, std::vector<symbol> const &key, std::vector<std::vector<symbol> > const &mappedTo);
private:
   std::vector<std::vector<symbol> > mMappedTo;
   std::vector<symbol> mKey;
   std::vector<symbol> mVariables, mConstants, * mString1, mString2; /* TRY IMPLEMENTING IT SO THAT I UPDATE BOTH STRINGS IN ONE UPDATE */
};
