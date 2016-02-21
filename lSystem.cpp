#include "lSystem.h"
#include <iostream>
#include <cassert>

symbol::symbol(std::vector<double> const &params, std::vector<condition> const &conditions, std::vector<mapping> const &mappings) {
   assert(((params.size() == conditions.size()) & (params.size() == mappings.size()))); /*For now we will have one cnd and mppng per param*/
   mParams = params;
   mConditions = conditions;
   mMappings = mappings;
}

void symbol::map() {
   for(int iParamNum = 0; iParamNum < mParams.size(); iParamNum++) {
      if(mConditions[iParamNum](mParams[iParamNum])) {
         mParams[iParamNum] = mMappings[iParamNum](mParams[iParamNum]); /*t -> m(t)*/
      } else {
         mParams[iParamNum] = 0; /*Later I will do something better here to handle this case*/
      }
   }
};

lSystem::lSystem(std::vector<symbol> const &axiom, std::vector<symbol> * endString, std::vector<symbol> const &variables, std::vector<symbol> const &constants, std::vector<symbol> const &key, std::vector<std::vector<symbol> > const &mappedTo) {
   mString1 = endString;
   mString2 = axiom;
   mVariables = variables;
   mConstants = constants;
   mKey = key;
   mMappedTo = mappedTo;
}
   

int main () {
   //Let's create some symbols first
   std::vector<double> params {20};
   std::vector<condition> conditions {[] (double t) {return t > 3; }};
   std::vector<mapping> mappings {[] (double t) {return t /= 2; }};
   symbol F(params, conditions, mappings);
   symbol G(params, conditions, mappings);
   for (int i = 0; i < 10; i++) {
      F.map();
   }

}
