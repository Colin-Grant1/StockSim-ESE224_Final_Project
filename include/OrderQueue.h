#ifndef ORDERQUEUE_H
#define ORDERQUEUE_H

#include <iostream>
#include <string>

using namespace std;

struct Order {
    string ticker;
    string type;          // "MARKET" or "LIMIT"
    string side;          // "BUY" or "SELL"
    double targetPrice;   // LIMIT target, 0 for MARKET
    int shares;
    string submittedDate;
};

struct QueueNode {
    Order data;
    QueueNode* next;

    QueueNode(const Order& order) : data(order), next(nullptr) {}
};

class OrderQueue {
private:
    QueueNode* front;
    QueueNode* rear;
    int size;

public:
    OrderQueue();
    ~OrderQueue();

    void enqueue(const Order& order);
    Order dequeue();
    Order peek() const;

    bool isEmpty() const;
    int getSize() const;

    void printAll() const;
};

#endif