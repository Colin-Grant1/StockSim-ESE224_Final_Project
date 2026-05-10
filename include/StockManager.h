#ifndef STOCKMANAGER_H
#define STOCKMANAGER_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

using namespace std;

template <typename T>
class StockManager {
private:
    vector<T*> assets;

public:
    StockManager() {}

    ~StockManager() {
        for (size_t i = 0; i < assets.size(); i++) {
            delete assets[i];
        }
        assets.clear();
    }

    void addAsset(T* asset) {
        if (asset != nullptr) {
            assets.push_back(asset);
        }
    }

    void removeAsset(const string& ticker) {
        for (typename vector<T*>::iterator it = assets.begin(); it != assets.end(); ++it) {
            if ((*it)->getTicker() == ticker) {
                delete *it;
                assets.erase(it);
                return;
            }
        }
    }

    T* findByTicker(const string& ticker) const {
        for (size_t i = 0; i < assets.size(); i++) {
            if (assets[i]->getTicker() == ticker) {
                return assets[i];
            }
        }
        return nullptr;
    }

    void sortByAnnualReturn(int year) {
        sort(assets.begin(), assets.end(),
            [year](T* a, T* b) {
                return a->calculateAnnualReturn(year) > b->calculateAnnualReturn(year);
            });
    }

    void sortByTicker() {
        sort(assets.begin(), assets.end(),
            [](T* a, T* b) {
                return a->getTicker() < b->getTicker();
            });
    }

    void printAll() const {
        if (assets.empty()) {
            cout << "No assets loaded.\n";
            return;
        }

        for (size_t i = 0; i < assets.size(); i++) {
            cout << "-----------------------------\n";
            assets[i]->printSummary();
            cout << endl;
        }
    }

    int getCount() const {
        return static_cast<int>(assets.size());
    }
};

#endif