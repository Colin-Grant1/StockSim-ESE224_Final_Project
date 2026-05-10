// CircularQueue.h
// Declares a fixed-size circular queue used for rolling window calculations.

#ifndef CIRCULARQUEUE_H
#define CIRCULARQUEUE_H

#include <iostream>
using namespace std;

class CircularQueue {
private:
    double* buffer;
    int capacity;
    int head;
    int tail;
    int count;

public:
    CircularQueue(int capacity);
    ~CircularQueue();

    void enqueue(double value);
    double dequeue();
    double peek() const;
    double getAverage() const;

    bool isFull() const;
    bool isEmpty() const;
    int getCount() const;
};

#endif