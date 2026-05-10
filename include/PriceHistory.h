// PriceHistory.h
// Declares linked-list storage and iterators for historical price data.

#ifndef PRICEHISTORY_H
#define PRICEHISTORY_H

#include <iostream>
#include <string>
#include "PriceNode.h"

using namespace std;

class PriceHistory {
private:
    PriceNode* head;
    PriceNode* tail;
    int size;

public:
    PriceHistory();
    ~PriceHistory();

    void append(const string& date, double open, double high, double low, double close, long volume);
    PriceNode* findByDate(const string& date) const;
    void printRange(const string& startDate, const string& endDate) const;
    int getSize() const;

    PriceNode* getHead() const;
    PriceNode* getTail() const;

    class Iterator {
    private:
        PriceNode* current;

    public:
        Iterator(PriceNode* node);
        Iterator& operator++();      // forward
        PriceNode& operator*();
        bool operator!=(const Iterator& other) const;
    };

    Iterator begin() const;
    Iterator end() const;

    class ReverseIterator {
    private:
        PriceNode* current;

    public:
        ReverseIterator(PriceNode* node);
        ReverseIterator& operator++();   // backward
        PriceNode& operator*();
        bool operator!=(const ReverseIterator& other) const;
    };

    ReverseIterator rbegin() const;
    ReverseIterator rend() const;
};

#endif