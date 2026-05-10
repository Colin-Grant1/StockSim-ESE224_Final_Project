// PriceHistory.cpp
// Implements linked-list price-history insertion, traversal, and range printing.

#include "../include/PriceHistory.h"
#include <iomanip>

PriceHistory::PriceHistory() {
    head = nullptr;
    tail = nullptr;
    size = 0;
}

PriceHistory::~PriceHistory() {
    PriceNode* current = head;
    while (current != nullptr) {
        PriceNode* temp = current;
        current = current->next;
        delete temp;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}

void PriceHistory::append(const string& date, double open, double high, double low, double close, long volume) {
    PriceNode* newNode = new PriceNode(date, open, high, low, close, volume);

    if (head == nullptr) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        newNode->prev = tail;
        tail = newNode;
    }

    size++;
}

PriceNode* PriceHistory::findByDate(const string& date) const {
    PriceNode* current = head;

    while (current != nullptr) {
        if (current->date == date) {
            return current;
        }
        current = current->next;
    }

    return nullptr;
}

void PriceHistory::printRange(const string& startDate, const string& endDate) const {
    PriceNode* current = head;

    cout << left
         << setw(12) << "Date"
         << setw(12) << "Open"
         << setw(12) << "High"
         << setw(12) << "Low"
         << setw(12) << "Close"
         << setw(15) << "Volume" << endl;

    cout << string(63, '-') << endl;

    while (current != nullptr) {
        if (current->date >= startDate && current->date <= endDate) {
            cout << left
                 << setw(12) << current->date
                 << setw(12) << fixed << setprecision(2) << current->open
                 << setw(12) << current->high
                 << setw(12) << current->low
                 << setw(12) << current->close
                 << setw(15) << current->volume
                 << endl;
        }
        current = current->next;
    }
}

int PriceHistory::getSize() const {
    return size;
}

PriceNode* PriceHistory::getHead() const {
    return head;
}

PriceNode* PriceHistory::getTail() const {
    return tail;
}

/* =========================
   Forward Iterator
   ========================= */

PriceHistory::Iterator::Iterator(PriceNode* node) {
    current = node;
}

PriceHistory::Iterator& PriceHistory::Iterator::operator++() {
    if (current != nullptr) {
        current = current->next;
    }
    return *this;
}

PriceNode& PriceHistory::Iterator::operator*() {
    return *current;
}

bool PriceHistory::Iterator::operator!=(const Iterator& other) const {
    return current != other.current;
}

PriceHistory::Iterator PriceHistory::begin() const {
    return Iterator(head);
}

PriceHistory::Iterator PriceHistory::end() const {
    return Iterator(nullptr);
}

/* =========================
   Reverse Iterator
   ========================= */

PriceHistory::ReverseIterator::ReverseIterator(PriceNode* node) {
    current = node;
}

PriceHistory::ReverseIterator& PriceHistory::ReverseIterator::operator++() {
    if (current != nullptr) {
        current = current->prev;
    }
    return *this;
}

PriceNode& PriceHistory::ReverseIterator::operator*() {
    return *current;
}

bool PriceHistory::ReverseIterator::operator!=(const ReverseIterator& other) const {
    return current != other.current;
}

PriceHistory::ReverseIterator PriceHistory::rbegin() const {
    return ReverseIterator(tail);
}

PriceHistory::ReverseIterator PriceHistory::rend() const {
    return ReverseIterator(nullptr);
}