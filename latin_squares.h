#include <stdint.h>
#ifndef __LATIN_SQUARES_H__
#define __LATIN_SQUARES_H__

class LS{
public:
  LS(const std::string&);
  LS(uint16_t);
  ~LS();
  uint16_t get_dimension();
  uint16_t get_entry(uint16_t, uint16_t);
  void set_entry(uint16_t, uint16_t, uint16_t);
  void print_me();
  class Traversal* next_traversal(class Traversal*);
  class TraversalNode* next_traversal_node(class TraversalNode*);
  class TraversalNode* get_all_traversals_for_value(uint16_t);
  class LS* next_orthogonal_LS();
  class LS* next_orthogonal_LS2();
private:
  class TraversalNode** traversal_node_array;
  class TraversalNode** traversal_node_state;
  uint16_t dimension;
  uint16_t** entries;
};

class Traversal{
public:
  Traversal(uint16_t, uint16_t value);
  ~Traversal();
  uint16_t length;
  uint16_t value;
  uint16_t* entries;
  uint16_t* permutation;
  void print_me();
  uint8_t is_consistent(class Traversal*);
private:
};

class TraversalNode{
public:
  TraversalNode(class Traversal*);
  class TraversalNode* parent;
  class TraversalNode* child;
  class Traversal* traversal;
  uint8_t is_consistent(class TraversalNode*);
private:
};

#include "latin_squares.cpp"
#endif