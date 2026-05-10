#ifndef TRADESTACK_H
#define TRADESTACK_H

#include <iostream>
#include <string>

using namespace std;

struct TradeRecord {
    string ticker;
    string date;
    double price;
    int shares;
    string action;   // "BUY" or "SELL"
    double totalCost;
};

struct StackNode {
    TradeRecord data;
    StackNode* next;

    StackNode(const TradeRecord& record) : data(record), next(nullptr) {}
};

class TradeStack {
private:
    StackNode* top;
    int size;

public:
    TradeStack();
    ~TradeStack();

    void push(const TradeRecord& record);
    TradeRecord pop();
    TradeRecord peek() const;

    bool isEmpty() const;
    int getSize() const;

    void printAll() const;
};

#endif