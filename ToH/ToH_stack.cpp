#ifndef ToH_STACK_CPP
#define ToH_STACK_CPP

#include "ToH_stack.h"

using namespace std;

class NODE;
class STACK;

STACK::STACK()
{
    head = NULL;
}

STACK::~STACK()
{
    while(head) pop();
}

void STACK::push(int in, int size, BITMAP* source)
{
    NODE* temp = head;
    head = new NODE;
    head->data = in;
    if(source)
    {
        // The visible disk is blitted onto the center of a rectangular bitmap whose size is calculated below.
        // This rectangle can then be printed onto the buffer at a fixed location while producing the pyramidal effect.
        head->ring = create_bitmap(screenwidth/3, int(0.74*screenheight/(size)) - int(0.07*(16-size)*(size-in)));
        rectfill(head->ring, 0, 0, head->ring->w, head->ring->h, 0xFF00FF); // Rectangle is first made transparent
        masked_stretch_blit(source, head->ring, 0, 0, source->w, source->h, (16-size)*(size-in) + 8, 0, head->ring->w - 2*(16-size)*(size-in) - 16, head->ring->h);
    }
    head->next = temp;
}

int STACK::pop()
{
    if(head)
    {
        int out = head->data;
        NODE* temp = head;
        destroy_bitmap(temp->ring);
        head = head->next;
        delete temp;
        temp = NULL;
        return out;
    }
    return 0;
}

void STACK::modnumber(int input)
{
    number = input;
}

int STACK::rtnnumber()
{
    return number;
}

void STACK::initialize(int size, BITMAP* sourceone, BITMAP* sourcetwo)
{
    for(int c = size; c >= 1; c--)
    {
        if(c%2 == 0) push(c, size, sourceone);
        else push (c, size, sourcetwo);
    }
}

void STACK::ptobuffer(BITMAP* buffer)
{
    NODE* node = head;
    NODE* node2 = head;
    int output;
    if(!node) return;
    vector<NODE*> pbuffer(1); // Vector buffer allows the stack to be printed in reverse order (down->up)
    int c = 1;
    while(node) // Enter data to buffer
    {
        pbuffer[c-1] = node;
        pbuffer.resize(++c);
        node = node->next;
    }

    int hctr = 37; // Height counter -- controls the height to which each disk is printed.
                   // The initial height of 37 represents the height of the base.

    for(c-=2; c >= 0; c--) // Bitmaps are printed to main buffer
    {
        hctr += int(1*pbuffer[c]->ring->h) - 1;
        draw_sprite(buffer, pbuffer[c]->ring, int(float(screenwidth/3)*(number-1)), screenheight-hctr);
    }
}

#endif
