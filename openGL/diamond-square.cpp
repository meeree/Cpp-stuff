#include <cmath>
#include <vector>
#include <iostream>

static struct {
   std::vector<float> vertices;
} info;

void DiamondSquare (int n, int top, float rand_max) {
   int index = 0;
   std::default_random_engine generator;
   std::uniform_int_distribution<int> distribution(1,6);
   for (int i = top/2; i > -top/2; i -= n) {
      for (int j = top/2; j > -top/2; j -= n) {
         if (i == 0) {
            info.vertices[index] = 0; index++;
            info.vertices[index] = distribution(generator); index++;
            info.vertices[index] = n/2; index++;
            info.vertices[index] = 
}

int main () {
   n = 3;
   for (int i = 0; i < 3*(4+5*pow(4, n-1)); i++) {
      info.vertices.push_back(0);
   }
   //Set initial vertices

