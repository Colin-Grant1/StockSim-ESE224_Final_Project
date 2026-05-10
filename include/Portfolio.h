#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "OrderQueue.h"
#include "TradeStack.h"

using namespace std;

struct Position {
    string ticker;
    int shares;
    double avgCostBasis;
    double currentPrice;
};

class Portfolio {
private:
    vector<Position> holdings;

    OrderQueue pendingOrders;
    TradeStack tradeHistory;

    double cashBalance;

public:
    Portfolio(double initialCash = 100000.0);

    void buyShares(const string& ticker,
                   int shares,
                   double price,
                   const string& date);

    void sellShares(const string& ticker,
                    int shares,
                    double price,
                    const string& date);

    void undoLastTrade();

    void queueOrder(const Order& order);

    void executeNextOrder(double currentPrice,
                          const string& currentDate);

    double getTotalValue() const;
    double getTotalReturn() const;

    void printHoldings() const;

    void sortHoldingsByReturn();
    void sortHoldingsByTicker();

    void printTradeHistory() const;
    void printPendingOrders() const;

    double getCashBalance() const;
};

#endif