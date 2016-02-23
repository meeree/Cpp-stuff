#include "lSystem.h"
#include "turtle.cpp"
#include "../generalGraphicsDirectory.cpp"
#include <algorithm>
#include <iostream>

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
            auto const &stringAddition = mRules[it-mVariables.begin()](params, mString2);
            mString2->insert(mString2->end(), stringAddition.begin(), stringAddition.end());
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
   lSysSymbol plus = {'+', {3.14/2}};
   lSysSymbol minus = {'-', {3.14/2}};
   lSysSymbol up = {'^', {3.14/2}};
   lSysSymbol down = {'V', {3.14/2}};
   lSysSymbol lbrack = {'['};
   lSysSymbol rbrack = {']'};

   std::vector<char> variables = {'F', 'A'}; /*Variables must be ordered the same as rules*/
   std::vector<char> constants = {plus.name, minus.name, up.name, down.name, lbrack.name, rbrack.name};
   lSysString string = {{'A', {0.3}}}, container = {};

   rule rule1 = [&] (std::vector<double> params, lSysString* string) {return (std::vector<lSysSymbol>){{'F', {params[0]}}, {'F', {params[0]}}}; };
   rule rule2 = [&] (std::vector<double> params, lSysString* string) {return (std::vector<lSysSymbol>){{'F', {params[0]}},lbrack,plus,{'A',{params[0]}},rbrack,lbrack,minus,{'A', {params[0]}},rbrack,lbrack,up,{'A', {params[0]}},rbrack,lbrack,down,{'A', {params[0]}},rbrack,{'A', {params[0]}}}; };

   std::vector<rule> rules = {rule1, rule2};
   lSystem test(string, &container, variables, constants, rules);

   turtle toitle({0, -5, 0}, {0, 1, 0});
   std::vector<GLfloat> vertices {0,-5,0}, colors {};
   std::vector<GLint> indices {};

   for (int iCounter = 0; iCounter < 4; iCounter++) {
      test.update();
   }

   for (auto const &sym: container) {
//      std::cout<<sym.name<<"{";
//      for (auto const &param: sym.params) {
//         std::cout<<param;
//      }
//      std::cout<<"}";
      if (sym.name=='F') {
         auto const &pos = toitle.mv(sym.params[0]);
         vertices.push_back(pos[0]);
         vertices.push_back(pos[1]);
         vertices.push_back(pos[2]);
         indices.push_back(vertices.size()-2);
         indices.push_back(vertices.size()-1);
         continue;
      }
      if (sym.name==']') {
         auto const &total = toitle.popMatrix();
         for (auto const &pos: total) {
            vertices.push_back(pos[0]);
            vertices.push_back(pos[1]);
            vertices.push_back(pos[2]);
            indices.push_back(vertices.size()-2);
            indices.push_back(vertices.size()-1);
         }
         continue;
      }

      switch(sym.name) {
         case '+':
            toitle.Rz(sym.params[0]);
            continue;
         case '-':
            toitle.Rz(-sym.params[0]);
            continue;
         case '^':
            toitle.Rx(sym.params[0]);
            continue;
         case 'V':
            toitle.Rx(-sym.params[0]);
            continue;
         case '[':
            toitle.pushMatrix();
            continue;
      }
   }
   std::cout<<std::endl;

   for (int iVert=0; iVert < vertices.size(); iVert++) {
      colors.push_back(1);
   }

   initializePrograms(700, 700, false, 0, 0, 0);
   glm::mat4 projection = setProjection(40);
   glm::mat4 view = setView(0, 0, -20);
   glm::mat4 model = glm::mat4(1.0f);
   GLuint shaderProgram = shaders("../shader/vert.shader", "../shader/frag.shader");
   GLuint vbo, vao;
   GLint PVM = createBuffers(vbo, vao, shaderProgram, colors, vertices, indices, true);
   while(true) {
      model *= glm::rotate(glm::radians(0.3f), glm::vec3(0.0f, 1.0f, 0.0f));
      draw(vbo, vao, indices.size(), GL_LINES, projection, view, model, PVM);
   }
}
