#include "../include/TradeStack.h"
#include <iomanip>

TradeStack::TradeStack() {
    top = nullptr;
    size = 0;
}

TradeStack::~TradeStack() {
    while (!isEmpty()) {
        pop();
    }
}

void TradeStack::push(const TradeRecord& record) {
    StackNode* newNode = new StackNode(record);
    newNode->next = top;
    top = newNode;
    size++;
}

TradeRecord TradeStack::pop() {
    if (isEmpty()) {
        return {"", "", 0.0, 0, "", 0.0};
    }

    StackNode* temp = top;
    TradeRecord record = temp->data;

    top = top->next;
    delete temp;
    size--;

    return record;
}

TradeRecord TradeStack::peek() const {
    if (isEmpty()) {
        return {"", "", 0.0, 0, "", 0.0};
    }

    return top->data;
}

bool TradeStack::isEmpty() const {
    return top == nullptr;
}

int TradeStack::getSize() const {
    return size;
}

void TradeStack::printAll() const {
    if (isEmpty()) {
        cout << "No trade history.\n";
        return;
    }

    cout << left
         << setw(10) << "Ticker"
         << setw(12) << "Date"
         << setw(10) << "Action"
         << setw(10) << "Shares"
         << setw(12) << "Price"
         << setw(12) << "Total" << endl;

    cout << string(66, '-') << endl;

    StackNode* current = top;
    while (current != nullptr) {
        const TradeRecord& t = current->data;

        cout << left
             << setw(10) << t.ticker
             << setw(12) << t.date
             << setw(10) << t.action
             << setw(10) << t.shares
             << setw(12) << fixed << setprecision(2) << t.price
             << setw(12) << t.totalCost
             << endl;

        current = current->next;
    }
}