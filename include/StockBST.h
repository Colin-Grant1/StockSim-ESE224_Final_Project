#ifndef STOCKBST_H
#define STOCKBST_H

#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct BSTNode {
    string ticker;
    double key;     // annual return % or volatility
    int year;

    BSTNode* left;
    BSTNode* right;

    BSTNode(const string& t, double k, int y)
        : ticker(t), key(k), year(y), left(nullptr), right(nullptr) {}
};

class StockBST {
private:
    BSTNode* root;

    BSTNode* insert(BSTNode* node, const string& ticker, double key, int year);
    BSTNode* search(BSTNode* node, double key) const;

    void inorder(BSTNode* node) const;
    void preorder(BSTNode* node) const;
    void postorder(BSTNode* node) const;

    void rangeSearch(BSTNode* node, double low, double high, vector<BSTNode*>& results) const;
    int getHeight(BSTNode* node) const;
    void clear(BSTNode* node);

public:
    StockBST();
    ~StockBST();

    void insert(const string& ticker, double key, int year);
    BSTNode* search(double key) const;

    void rangeSearch(double low, double high, vector<BSTNode*>& results) const;

    void inorder() const;
    void preorder() const;
    void postorder() const;

    int getHeight() const;
    void clear();
};

#endif