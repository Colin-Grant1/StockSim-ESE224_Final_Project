// StockBST.cpp
// Implements BST insertion, traversal, range search, and extremum lookup.

#include "../include/StockBST.h"
#include <iomanip>

StockBST::StockBST() {
    root = nullptr;
}

StockBST::~StockBST() {
    clear();
}

BSTNode* StockBST::insert(BSTNode* node, const string& ticker, double key, int year) {
    if (node == nullptr) {
        return new BSTNode(ticker, key, year);
    }

    if (key < node->key) {
        node->left = insert(node->left, ticker, key, year);
    } else {
        node->right = insert(node->right, ticker, key, year);
    }

    return node;
}

void StockBST::insert(const string& ticker, double key, int year) {
    root = insert(root, ticker, key, year);
}

BSTNode* StockBST::search(BSTNode* node, double key) const {
    if (node == nullptr || node->key == key) {
        return node;
    }

    if (key < node->key) {
        return search(node->left, key);
    }

    return search(node->right, key);
}

BSTNode* StockBST::search(double key) const {
    return search(root, key);
}

void StockBST::inorder(BSTNode* node) const {
    if (node == nullptr) return;

    inorder(node->left);

    cout << left
         << setw(10) << node->ticker
         << setw(12) << fixed << setprecision(2) << node->key
         << setw(8) << node->year << endl;

    inorder(node->right);
}

void StockBST::preorder(BSTNode* node) const {
    if (node == nullptr) return;

    cout << left
         << setw(10) << node->ticker
         << setw(12) << fixed << setprecision(2) << node->key
         << setw(8) << node->year << endl;

    preorder(node->left);
    preorder(node->right);
}

void StockBST::postorder(BSTNode* node) const {
    if (node == nullptr) return;

    postorder(node->left);
    postorder(node->right);

    cout << left
         << setw(10) << node->ticker
         << setw(12) << fixed << setprecision(2) << node->key
         << setw(8) << node->year << endl;
}

void StockBST::rangeSearch(BSTNode* node, double low, double high, vector<BSTNode*>& results) const {
    if (node == nullptr) return;

    if (low < node->key) {
        rangeSearch(node->left, low, high, results);
    }

    if (node->key >= low && node->key <= high) {
        results.push_back(node);
    }

    if (high > node->key) {
        rangeSearch(node->right, low, high, results);
    }
}

void StockBST::rangeSearch(double low, double high, vector<BSTNode*>& results) const {
    rangeSearch(root, low, high, results);
}

int StockBST::getHeight(BSTNode* node) const {
    if (node == nullptr) return 0;

    int leftHeight = getHeight(node->left);
    int rightHeight = getHeight(node->right);

    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

int StockBST::getHeight() const {
    return getHeight(root);
}

void StockBST::clear(BSTNode* node) {
    if (node == nullptr) return;

    clear(node->left);
    clear(node->right);
    delete node;
}

void StockBST::clear() {
    clear(root);
    root = nullptr;
}

void StockBST::inorder() const {
    if (root == nullptr) {
        cout << "BST is empty.\n";
        return;
    }

    cout << left
         << setw(10) << "Ticker"
         << setw(12) << "Key"
         << setw(8) << "Year" << endl;

    cout << string(30, '-') << endl;
    inorder(root);
}

void StockBST::preorder() const {
    if (root == nullptr) {
        cout << "BST is empty.\n";
        return;
    }

    cout << left
         << setw(10) << "Ticker"
         << setw(12) << "Key"
         << setw(8) << "Year" << endl;

    cout << string(30, '-') << endl;
    preorder(root);
}

void StockBST::postorder() const {
    if (root == nullptr) {
        cout << "BST is empty.\n";
        return;
    }

    cout << left
         << setw(10) << "Ticker"
         << setw(12) << "Key"
         << setw(8) << "Year" << endl;

    cout << string(30, '-') << endl;
    postorder(root);
}