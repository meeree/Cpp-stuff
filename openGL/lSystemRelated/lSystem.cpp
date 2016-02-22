#include <iostream>
#include "lSystem.h"
#include <algorithm>

lSystem::lSystem(lSysString const &string, lSysString * endStringContainer, std::vector<char> const &variables, std::vector<char> const &constants, std::vector<rule> const &rules) {
   mString1 = string;
   mString2 = endStringContainer;
   mVariables = variables;
   mConstants = constants;
   mRules = rules;
}

void lSystem::update() {
   mString2->clear();
   for (auto const &symbol: mString1) {
      char const &name = symbol.name;
      std::vector<double> const &params = symbol.params;
      if (std::find(mConstants.begin(), mConstants.end(), name) != mConstants.end()) {
         mString2->push_back(symbol);
      } else {
         auto const &it = std::find(mVariables.begin(), mVariables.end(), name);
         if (it != mVariables.end()) {
            mRules[it-mVariables.begin()](params, mString2);
         }
      }
   }
   mString1.clear();
   mString1.resize(mString2->size());
   for (int iSymbol = 0; iSymbol < mString2->size(); iSymbol++) {
      mString1[iSymbol] = (*mString2)[iSymbol];
   }
}

int main () {
   std::vector<char> variables = {'A', 'B'}; /*Variables must be ordered the same as rules*/
   std::vector<char> constants = {};
   lSysString string = {{'A', {1}}}, container = {};

   rule rule1 = [] (std::vector<double> params, lSysString* string) {string->push_back({'A', {params[0]/2}});string->push_back({'B', {params[0]/2}}); };
   rule rule2 = [] (std::vector<double> params, lSysString* string) {string->push_back({'A', {params[0]}}); };

   std::vector<rule> rules = {rule1, rule2};
   lSystem test(string, &container, variables, constants, rules);

   for (int k = 0; k < 5; k++) {
      test.update();
      for (int i = 0; i < container.size(); i++) {
         std::cout<<container[i].name<<"(";
         for (auto const &j: container[i].params) {
            std::cout<<j<<",";
         }
         std::cout<<")";
      } std::cout<<std::endl;
   }
}

