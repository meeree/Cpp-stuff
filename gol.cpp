#define rows 30
#define columns 60

#include <unistd.h>
#include <cmath>
#include <iostream>
#include <vector>

bool outOfBounds (int const &row, int const &col) {
   if ((row == rows) or (row == -1) or (col == columns) or (col == -1)) {
      return true;
   }
   return false;
}

void gol (int (&board1)[rows][columns], int (&board2)[rows][columns]) {
   int surroundings;
   for (int row=0; row < rows; row++) {
      for (int col=0; col < columns; col++) {
         bool const &alive = board1[row][col];

         surroundings = 0;

         for (int nextCol=col-1; nextCol < col+2; nextCol++) {
            if (!outOfBounds(row-1, nextCol)) {
               surroundings += board1[row-1][nextCol];
            }
            if (nextCol != col) {
               if (!outOfBounds(row, nextCol)) {
                  surroundings += board1[row][nextCol];
               }
            }
            if (!outOfBounds(row+1, nextCol)) {
               surroundings += board1[row+1][nextCol];
            }
         }

         if (!alive) {
            if (surroundings == 3) {
               board2[row][col] = true;
            } else {
               board2[row][col] = false;
            }
         } else {
            if ((surroundings != 3) and (surroundings != 2)) {
               board2[row][col] = false;
            } else {
               board2[row][col] = true;
            }
         }
      }
   }
   
   for (int row=0; row < rows; row++) {
      for (int col=0; col < columns; col++) {
         board1[row][col] = board2[row][col];
      }
   }
}

int main () {
   int board1[rows][columns], board2[rows][columns];
   for (int row = 0; row < rows; row++) {
      for (int col=0; col < columns; col++) {
         board1[row][col] = 0;
         board2[row][col] = 0;
      }
   }

   std::vector<int> vars = {5, 1, 5, 2, 6, 2, 6, 1, 5, 11, 6, 11, 7, 11, 4, 12, 8, 12, 9, 13, 3, 13, 3, 14, 9, 14, 6, 15, 8, 16, 4, 16, 5, 17, 6, 17, 7, 17, 6, 18, 5, 21, 4, 21, 3, 21, 5, 22, 4, 22, 3, 22, 2, 23, 6, 23, 6, 25, 7, 25, 2, 25, 1, 25, 3, 36, 3, 37, 4, 36, 4, 37};
//   std::vector<int> vars = {2, 0, 2, 1, 2, 2, 1, 2, 0, 1};
   for (int index = 0; index < vars.size()-1; index += 2) {
      board1[vars[index]][vars[index+1]] = true;
   }

   for (int i = 0; i < 20000; i++) {
      for (int row=0; row < rows; row++) {
         std::cout<<"|";
         for (int col=0; col < columns; col++) {
            if (board1[row][col]) {
               std::cout<<"#|";
            } else {
               std::cout<<"-|";
            } 
         }
         std::cout<<std::endl;
      }
      std::cout<<std::endl;
      gol(board1, board2);
//      usleep(300000);
   }

}
