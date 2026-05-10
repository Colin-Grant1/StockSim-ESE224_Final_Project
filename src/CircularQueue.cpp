#include "../include/CircularQueue.h"

CircularQueue::CircularQueue(int capacity) {
    this->capacity = capacity;
    buffer = new double[capacity];

    head = 0;
    tail = 0;
    count = 0;
}

CircularQueue::~CircularQueue() {
    delete[] buffer;
}

void CircularQueue::enqueue(double value) {
    if (isFull()) {
        // overwrite oldest
        buffer[tail] = value;
        tail = (tail + 1) % capacity;
        head = tail;
    } else {
        buffer[tail] = value;
        tail = (tail + 1) % capacity;
        count++;
    }
}

double CircularQueue::dequeue() {
    if (isEmpty()) {
        return 0.0;
    }

    double value = buffer[head];
    head = (head + 1) % capacity;
    count--;

    return value;
}

double CircularQueue::peek() const {
    if (isEmpty()) {
        return 0.0;
    }

    return buffer[head];
}

double CircularQueue::getAverage() const {
    if (isEmpty()) {
        return 0.0;
    }

    double sum = 0.0;
    int index = head;

    for (int i = 0; i < count; i++) {
        sum += buffer[index];
        index = (index + 1) % capacity;
    }

    return sum / count;
}

bool CircularQueue::isFull() const {
    return count == capacity;
}

bool CircularQueue::isEmpty() const {
    return count == 0;
}

int CircularQueue::getCount() const {
    return count;
}