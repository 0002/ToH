#ifndef ToH_STACK_H
#define ToH_STACK_H

#include <iostream>
#include <cstdlib>
#include <vector>
#include <allegro.h>
#include <cmath>

using namespace std;

#define screenwidth 540
#define screenheight 300

class NODE // Modified to incorporate bitmaps
{
  public:
    int data; // Represents each disk's size
    BITMAP* ring; // Holds the disk's bitmap
    NODE* next;
};

class STACK
{
  private:
    NODE* head;
    int number; // Identifies the stack as 1 (starting location), 2, or 3 (ending location)
  public:
    void push(int in, int size, BITMAP* source);
    int pop();
    void initialize(int size, BITMAP* sourceone, BITMAP* sourcetwo); // Initialize the stack with alternating bitmaps
    void ptobuffer(BITMAP* buffer); // Prints stack to buffer
    void modnumber(int in); // Modifier function
    int rtnnumber(); // Accessor function

    STACK();
    ~STACK();
};

#include "ToH_stack.cpp"
#endif
