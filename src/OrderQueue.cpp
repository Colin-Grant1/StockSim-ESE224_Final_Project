// OrderQueue.cpp
// Implements FIFO pending-order queue operations.

#include "../include/OrderQueue.h"
#include <iomanip>

OrderQueue::OrderQueue() {
    front = nullptr;
    rear = nullptr;
    size = 0;
}

OrderQueue::~OrderQueue() {
    while (!isEmpty()) {
        dequeue();
    }
}

void OrderQueue::enqueue(const Order& order) {
    QueueNode* newNode = new QueueNode(order);

    if (isEmpty()) {
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }

    size++;
}

Order OrderQueue::dequeue() {
    if (isEmpty()) {
        return {"", "", "", 0.0, 0, ""};
    }

    QueueNode* temp = front;
    Order order = temp->data;

    front = front->next;
    if (front == nullptr) {
        rear = nullptr;
    }

    delete temp;
    size--;

    return order;
}

Order OrderQueue::peek() const {
    if (isEmpty()) {
        return {"", "", "", 0.0, 0, ""};
    }

    return front->data;
}

bool OrderQueue::isEmpty() const {
    return front == nullptr;
}

int OrderQueue::getSize() const {
    return size;
}

void OrderQueue::printAll() const {
    if (isEmpty()) {
        cout << "No pending orders.\n";
        return;
    }

    cout << left
         << setw(10) << "Ticker"
         << setw(10) << "Type"
         << setw(10) << "Side"
         << setw(12) << "Target"
         << setw(10) << "Shares"
         << setw(15) << "Submitted" << endl;

    cout << string(67, '-') << endl;

    QueueNode* current = front;
    while (current != nullptr) {
        const Order& o = current->data;

        cout << left
             << setw(10) << o.ticker
             << setw(10) << o.type
             << setw(10) << o.side
             << setw(12) << fixed << setprecision(2) << o.targetPrice
             << setw(10) << o.shares
             << setw(15) << o.submittedDate
             << endl;

        current = current->next;
    }
}