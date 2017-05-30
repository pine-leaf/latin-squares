/*

*/

#ifndef __LATIN_SQUARES_CPP__
#define __LATIN_SQUARES_CPP__
#include "latin_squares.h"
//#include "graphs.h"
#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <omp.h>
#include <math.h>

extern uint64_t num_traversals = 0ULL;
extern uint64_t num_LS = 0ULL;
extern uint64_t num_TN = 0ULL;
extern uint64_t num_deletes = 0ULL;
extern uint64_t next_traversal_called = 0ULL;

LS::LS(const std::string& fileName){
  num_LS++;
  this->traversal_node_array = NULL;
  this->traversal_node_state = NULL;
  std::string word;
  std::string line;
  std::ifstream file;
  file.open(fileName);
  const char* c_string;
  if(!file.is_open()){
    std::cout << "LS() Error:\n  Coudln't open file:\n  " << fileName << std::endl;
    return;
  }
  this->dimension = 0;
  this->entries = NULL;
  uint16_t line_number = 0;
  uint16_t i,j;
  while(std::getline(file, line)){
    std::stringstream iss_line(line);
    if(!(iss_line >> word)){continue;}
    c_string = word.c_str();
    if(!strcmp(c_string, "D:")){
      iss_line >> word;
      c_string = word.c_str();
      this->dimension = (uint16_t)atoi(c_string);
      if(!this->dimension){
        return;
      }
      this->entries = new uint16_t*[this->dimension];
      for(i = 0; i < this->dimension; i++){
        this->entries[i] = new uint16_t[this->dimension];
      }
      continue;
    }
    if(line_number >= this->dimension){
      break;
    }
    if(this->dimension){
      this->entries[line_number][0] = (uint16_t) atoi(c_string);
      for(i = 1; i < this->dimension; i++){
        if(!(iss_line >> word)){continue;}
        c_string = word.c_str();
        this->entries[line_number][i] = (uint16_t) atoi(c_string);
      }
      line_number++;
    }
  }
}

LS::LS(uint16_t dimension){
  num_LS++;
  this->dimension = dimension;
  this->traversal_node_array = NULL;
  this->traversal_node_state = NULL;
  uint16_t i,j;
  this->entries = new uint16_t*[dimension];
  for(i = 0; i < dimension; i++){
    this->entries[i] = new uint16_t[dimension];
    for(j = 0; j < dimension; j++){
      this->entries[i][j] = 0;
    }
  }
}

LS::~LS(){
  uint16_t i;
  if(!this->entries){
    return;
  }
  for(i = 0; i < this->dimension; i++){
    if(this->entries[i]){
      delete this->entries[i];
    }
  }
  delete this->entries;
  if(this->traversal_node_array){
    delete this->traversal_node_array;
  }
  if(this->traversal_node_state){
    delete this->traversal_node_state;
  }
}

void LS::print_me(){
  uint16_t i,j;
  for(i = 0; i < this->dimension; i++){
    for(j = 0; j < this->dimension; j++){
      printf("%3u",this->entries[i][j]);
    }
    printf("\n");
  }
}

uint16_t LS::get_dimension(){
  return this->dimension;
}

uint16_t LS::get_entry(uint16_t i, uint16_t j){
  if((i >= this->dimension) || (j >= this->dimension)){
    return 0;
  }
  return this->entries[i][j];
}

void LS::set_entry(uint16_t i, uint16_t j, uint16_t value){
  if((i >= this->dimension) || (j >= this->dimension)){
    return;
  }
  this->entries[i][j] = value;  
}

Traversal::Traversal(uint16_t length, uint16_t value){
  num_traversals++;
  this->length = length;
  this->value = value;
  this->entries = new uint16_t[length];
  this->permutation = new uint16_t[length];
  uint16_t i;
  for(i = 0; i < length; i++){
    this->entries[i] = 0;
    this->permutation[i] = 0;
  }
}

Traversal::~Traversal(){
  num_deletes++;
  delete this->entries;
  delete this->permutation;
}

class Traversal* LS::next_traversal(class Traversal* T){
  next_traversal_called++;
  class LS* L = this;
  if(!T){
    return NULL;
  }
  uint8_t* occupied_values = new uint8_t[L->dimension + 1];
  uint8_t* occupied_spots = new uint8_t[L->dimension + 1];
  uint16_t i,j,k;
  uint16_t value;
  uint8_t set_something;
  uint8_t consistent;
  if(!L->dimension){
    delete occupied_values;
    delete occupied_spots;
    return NULL;
  }
  //std::cout << "LS::next_traversal() called." << std::endl;
  for(i = 0; i <= this->dimension; i++){
    occupied_values[i] = 0;
    occupied_spots[i] = 0;
  }
  class Traversal* tmp;
  class Traversal* output = new Traversal(L->dimension, T->value);
  i = 0;
  if(T->entries[0]){
    //std::cout << "Try to incrememt." << std::endl;
    for(i = 0; i < L->dimension; i++){
      output->entries[i] = T->entries[i];
      output->permutation[i] = T->permutation[i];
      occupied_values[output->entries[i]] = 1;
      occupied_spots[output->permutation[i]] = 1;
    }
    i = L->dimension-1;
  }
  else{
    //std::cout << "Try to make the first one." << std::endl;
    occupied_values[L->entries[0][T->value]] = 1;
    occupied_spots[T->value] = 1;
    output->permutation[0] = T->value;
    output->entries[0] = L->entries[0][T->value];
    i = 1;
  }
  while((i < L->dimension) && (i > 0)){
    set_something = 0;
    if(output->entries[i]){
      // This was already set, so we'll have to unset it, and act accordingly.
      occupied_values[output->entries[i]] = 0;
      occupied_spots[output->permutation[i]] = 0;
      j = output->permutation[i] + 1;
    }
    else{
      j = 0;
    }
    for(; j < L->dimension; j++){
      value = L->entries[i][j];
      consistent = 1;
      if(this->traversal_node_state){
        for(k = 0; k < T->value; k++){
          if(this->traversal_node_state[k]){
            tmp = this->traversal_node_state[k]->traversal;
            if(tmp){
              if(tmp->entries){
                if(tmp->entries[i] == value){
                  consistent = 0;
                  break;
                }
              }
              else{
                // Do something?
              }
            }
            else{
              // Do something?
            }
          }
        }
      }
      if(consistent){
        if((!occupied_values[value]) && (!occupied_spots[j])){
          occupied_values[value] = 1;
          occupied_spots[j] = 1;
          output->permutation[i] = j;
          output->entries[i] = value;
          set_something = 1;
          break;
        }
      }
    }
    if(set_something){
      i++;
    }
    else{
      if(output->entries[i]){
        // If it had been set previously.
        occupied_spots[output->permutation[i]] = 0;
        occupied_values[output->entries[i]] = 0;
        output->permutation[i] = 0;
        output->entries[i] = 0;
      }
      i--;
      if(i < 1){
        delete output;
        delete occupied_values;
        delete occupied_spots;
        return NULL;
      }
    }
  }

  delete occupied_values;
  delete occupied_spots;
  return output;
}

class TraversalNode* LS::next_traversal_node(class TraversalNode* N){
  if(!N){
    return NULL;
  }
  class Traversal* T = this->next_traversal(N->traversal);
  if(!T){
    return NULL;
  }
  return new TraversalNode(T);
}

void Traversal::print_me(){
  uint16_t i;
  std::cout << "value: " << this->value << std::endl;
  std::cout << "permutation:" << std::endl;
  for(i = 0; i < this->length; i++){
    std::cout << this->permutation[i] << " ";
  }
  std::cout << std::endl;
  std::cout << "entries:" << std::endl;
  for(i = 0; i < this->length; i++){
    std::cout << this->entries[i] << " ";
  }
  std::cout << std::endl;
}

TraversalNode::TraversalNode(class Traversal* T){
  num_TN++;
  this->parent = NULL;
  this->child = NULL;
  this->traversal = T;
}

uint8_t Traversal::is_consistent(class Traversal* B){
  class Traversal* A = this;
  if(!B){
    return 0;
  }
  uint16_t i;
  if(A->value == B->value){
    return 0;
  }
  if(A->length != B->length){
    return 0;
  }
  for(i = 0; i < A->length; i++){
    if(A->permutation[i] == B->permutation[i]){
      return 0;
    }
    if(A->entries[i] == B->entries[i]){
      return 0;
    }
  }
  return 1;
}

uint8_t TraversalNode::is_consistent(class TraversalNode* B){
  class TraversalNode* A = this;
  if(!B){
    return 0;
  }
  if(!A->traversal){
    return 0;
  }
  if(!B->traversal){
    return 0;
  }
  return A->traversal->is_consistent(B->traversal);
}

class TraversalNode* LS::get_all_traversals_for_value(uint16_t value){
  class TraversalNode* N = NULL;
  class TraversalNode* H = NULL;
  class Traversal T(this->dimension, value);
  class Traversal* A = this->next_traversal(&T);
  uint64_t counter = 0;
  while(A){
    counter++;
    N = new TraversalNode(A);
    N->child = H;
    if(H){
      H->parent = N;
    }
    H = N;
    A = this->next_traversal(A);
  }
  std::cout << "value: " << value << " has: " << counter << " transversals." << std::endl;
  std::cout << "num_traversals: " << num_traversals << std::endl;
  return H;
}
/*
class LS* LS::next_orthogonal_LS(){
  class LS* output = NULL;
  uint8_t consistent;
  int i,j;
  uint16_t dim = this->dimension;
  class TraversalNode** array;
  class TraversalNode** state;
  if(!this->traversal_node_state){
    this->traversal_node_state = new TraversalNode*[dim];
    this->traversal_node_array = new TraversalNode*[dim];
    array = this->traversal_node_array;
    state = this->traversal_node_state;
    
    #pragma omp parallel for shared(array, state)
    for(i = 0; i < dim; i++){
      array[i] = this->get_all_traversals_for_value(i);
      //array[i] = NULL;
      state[i] = NULL;
    }

    i = 0;
  }
  else{
    array = this->traversal_node_array;
    state = this->traversal_node_state;
    i = this->dimension-1;
  }

  while((i < this->dimension) && (i >= 0)){
    if(!state[i]){
      state[i] = array[i];
      if(!array[i]){
        return NULL;
      }
    }
    else{
      state[i] = state[i]->child;
    }
    consistent = 0;
    while(state[i]){
      consistent = 1;
      for(j = 0; j < i; j++){
        if(!state[i]->is_consistent(state[j])){
          consistent = 0;
          break;
        }
      }
      if(consistent){
        break;
      }
      state[i] = state[i]->child;
    }
    if(consistent){
      i++;
    }
    else{
      i--;
    }
  }
  if(i < 0){
    return NULL;
  }
  output = new LS(dim);
  class Traversal* T;
  for(i = 0; i < dim; i++){
    T = state[i]->traversal;
    for(j = 0; j < dim; j++){
      output->entries[i][j] = T->entries[j];
    }
  }
  std::cout << "num_TN " << num_TN << std::endl;
  std::cout << "num_traversals " << num_traversals << std::endl;
  std::cout << "num_LS " << num_LS << std::endl;
  return output;
}*/


class LS* LS::next_orthogonal_LS(){

  class LS* output = NULL;
  uint8_t consistent;
  int i,j;
  int max_i = 0;
  uint64_t counter = 0ULL;
  uint16_t dim = this->dimension;
  class TraversalNode** array;
  class TraversalNode** state;
  if(!this->traversal_node_state){
    this->traversal_node_state = new TraversalNode*[dim];
    this->traversal_node_array = new TraversalNode*[dim];
    array = this->traversal_node_array;
    state = this->traversal_node_state;
    
    #pragma omp parallel for shared(array, state)
    for(i = 0; i < dim; i++){
      //array[i] = this->get_all_traversals_for_value(i);
      array[i] = NULL;
      state[i] = NULL;
    }

    i = 0;
  }
  else{
    array = this->traversal_node_array;
    state = this->traversal_node_state;
    i = this->dimension-1;
  }

  //std::cout << "Here 0 " << std::endl;
  class TraversalNode* N;
  class Traversal* tmp;
  while((i < this->dimension) && (i >= 0)){
    counter++;
    if(i > max_i){
      max_i = i;
      std::cout << "max_i: " << max_i << " counter: " << counter << std::endl;
    }
    if(!state[i]){
      tmp = new Traversal(this->dimension, i);
      N = new TraversalNode(tmp);
      state[i] = this->next_traversal_node(N);
      delete tmp;
      delete N;
    }
    else{
      N = state[i];
      state[i] = this->next_traversal_node(N);
      if(N->traversal){
        delete N->traversal;
      }
      delete N;
    }
    consistent = 0;
    while(state[i]){
      counter++;
      consistent = 1;
      for(j = 0; j < i; j++){
        if(!state[i]->is_consistent(state[j])){
          consistent = 0;
          break;
        }
      }
      if(consistent){
        break;
      }
      N = state[i];
      state[i] = this->next_traversal_node(N);
      if(N->traversal){
        delete N->traversal;
      }
      delete N;
    }
    if(consistent){
      i++;
    }
    else{
      i--;
    }
  }
  if(i < 0){
    return NULL;
  }
  output = new LS(dim);
  class Traversal* T;
  for(i = 0; i < dim; i++){
    T = state[i]->traversal;
    for(j = 0; j < dim; j++){
      output->entries[i][j] = T->entries[j];
    }
  }
  std::cout << "num_TN " << num_TN << std::endl;
  std::cout << "num_traversals " << num_traversals << std::endl;
  std::cout << "num_LS " << num_LS << std::endl;
  return output;
}

#endif