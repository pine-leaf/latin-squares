#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "latin_squares.h"

using namespace std;

int main(){
  LS* L = new LS("latin_square.in");
  LS* N = L->next_orthogonal_LS2();
  if(L){
    cout << "Latin Square 0:" << endl;
    L->print_me();
    delete L;
  }
  else{
    cout << "L is NULL." << endl;
  }
  if(N){
    cout << "Latin Square 1:" << endl;
    N->print_me();
    delete N;
  }else{
    cout << "N is NULL." << endl;
  }

  return 0;
}