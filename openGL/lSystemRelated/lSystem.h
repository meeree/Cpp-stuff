#ifndef vector 
#include <vector> 
#endif 
#ifndef string 
#include <string> 
#endif 
#ifndef functional
#include <functional> 
#endif 

struct lSysSymbol {
   char name;
   std::vector<double> params;
};

typedef std::vector<lSysSymbol> lSysString;

typedef std::function<std::vector<lSysSymbol> (std::vector<double>, lSysString*)> rule;

class lSystem {
public:
   lSystem(lSysString const &string, lSysString * endStringContainer, std::vector<char> const &variables, std::vector<char> const &constants, std::vector<rule> const &rules);
   void update();

private:
   lSysString mString1;
   lSysString * mString2;
   std::vector<char> mVariables, mConstants;
   std::vector<rule> mRules;
};
