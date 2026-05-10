// Portfolio.cpp
// Implements portfolio accounting, trading actions, and summaries.

#include "../include/Portfolio.h"
#include <iomanip>

Portfolio::Portfolio(double initialCash) {
    cashBalance = initialCash;
}

void Portfolio::buyShares(const string& ticker,
                          int shares,
                          double price,
                          const string& date) {

    double totalCost = shares * price;

    if (totalCost > cashBalance) {
        cout << "Insufficient cash balance.\n";
        return;
    }

    cashBalance -= totalCost;

    bool found = false;

    for (size_t i = 0; i < holdings.size(); i++) {

        if (holdings[i].ticker == ticker) {

            double oldTotal =
                holdings[i].shares * holdings[i].avgCostBasis;

            double newTotal =
                shares * price;

            holdings[i].shares += shares;

            holdings[i].avgCostBasis =
                (oldTotal + newTotal) / holdings[i].shares;

            holdings[i].currentPrice = price;

            found = true;
            break;
        }
    }

    if (!found) {
        Position p;
        p.ticker = ticker;
        p.shares = shares;
        p.avgCostBasis = price;
        p.currentPrice = price;

        holdings.push_back(p);
    }

    TradeRecord record;
    record.ticker = ticker;
    record.date = date;
    record.price = price;
    record.shares = shares;
    record.action = "BUY";
    record.totalCost = totalCost;

    tradeHistory.push(record);

    cout << "Bought " << shares
         << " shares of " << ticker << endl;
}

void Portfolio::sellShares(const string& ticker,
                           int shares,
                           double price,
                           const string& date) {

    for (size_t i = 0; i < holdings.size(); i++) {

        if (holdings[i].ticker == ticker) {

            if (shares > holdings[i].shares) {
                cout << "Not enough shares to sell.\n";
                return;
            }

            double totalValue = shares * price;

            holdings[i].shares -= shares;
            holdings[i].currentPrice = price;

            cashBalance += totalValue;

            TradeRecord record;
            record.ticker = ticker;
            record.date = date;
            record.price = price;
            record.shares = shares;
            record.action = "SELL";
            record.totalCost = totalValue;

            tradeHistory.push(record);

            if (holdings[i].shares == 0) {
                holdings.erase(holdings.begin() + i);
            }

            cout << "Sold " << shares
                 << " shares of " << ticker << endl;

            return;
        }
    }

    cout << "Ticker not found in portfolio.\n";
}

void Portfolio::undoLastTrade() {

    if (tradeHistory.isEmpty()) {
        cout << "No trades to undo.\n";
        return;
    }

    TradeRecord last = tradeHistory.pop();

    if (last.action == "BUY") {

        for (size_t i = 0; i < holdings.size(); i++) {

            if (holdings[i].ticker == last.ticker) {

                holdings[i].shares -= last.shares;

                cashBalance += last.totalCost;

                if (holdings[i].shares <= 0) {
                    holdings.erase(holdings.begin() + i);
                }

                cout << "Undo BUY completed.\n";
                return;
            }
        }
    }

    else if (last.action == "SELL") {

        bool found = false;

        for (size_t i = 0; i < holdings.size(); i++) {

            if (holdings[i].ticker == last.ticker) {

                holdings[i].shares += last.shares;

                cashBalance -= last.totalCost;

                found = true;
                break;
            }
        }

        if (!found) {

            Position p;
            p.ticker = last.ticker;
            p.shares = last.shares;
            p.avgCostBasis = last.price;
            p.currentPrice = last.price;

            holdings.push_back(p);

            cashBalance -= last.totalCost;
        }

        cout << "Undo SELL completed.\n";
    }
}

void Portfolio::queueOrder(const Order& order) {
    pendingOrders.enqueue(order);
}

void Portfolio::executeNextOrder(double currentPrice,
                                 const string& currentDate) {

    if (pendingOrders.isEmpty()) {
        cout << "No pending orders.\n";
        return;
    }

    Order order = pendingOrders.peek();

    bool execute = false;

    if (order.type == "MARKET") {
        execute = true;
    }

    else if (order.type == "LIMIT") {

        if (order.side == "BUY" &&
            currentPrice <= order.targetPrice) {
            execute = true;
        }

        else if (order.side == "SELL" &&
                 currentPrice >= order.targetPrice) {
            execute = true;
        }
    }

    if (!execute) {
        cout << "Order conditions not met.\n";
        return;
    }

    pendingOrders.dequeue();

    if (order.side == "BUY") {

        buyShares(order.ticker,
                  order.shares,
                  currentPrice,
                  currentDate);
    }

    else if (order.side == "SELL") {

        sellShares(order.ticker,
                   order.shares,
                   currentPrice,
                   currentDate);
    }

    cout << "Order executed.\n";
}

double Portfolio::getTotalValue() const {

    double total = cashBalance;

    for (size_t i = 0; i < holdings.size(); i++) {
        total += holdings[i].shares *
                 holdings[i].currentPrice;
    }

    return total;
}

double Portfolio::getTotalReturn() const {

    double invested = 0.0;
    double current = 0.0;

    for (size_t i = 0; i < holdings.size(); i++) {

        invested += holdings[i].shares *
                    holdings[i].avgCostBasis;

        current += holdings[i].shares *
                   holdings[i].currentPrice;
    }

    if (invested == 0.0) return 0.0;

    return ((current - invested) / invested) * 100.0;
}

void Portfolio::printHoldings() const {

    if (holdings.empty()) {
        cout << "Portfolio is empty.\n";
        return;
    }

    cout << left
         << setw(10) << "Ticker"
         << setw(10) << "Shares"
         << setw(15) << "Avg Cost"
         << setw(15) << "Current"
         << setw(15) << "Return %" << endl;

    cout << string(65, '-') << endl;

    for (size_t i = 0; i < holdings.size(); i++) {

        double ret =
            ((holdings[i].currentPrice -
              holdings[i].avgCostBasis)
              / holdings[i].avgCostBasis) * 100.0;

        cout << left
             << setw(10) << holdings[i].ticker
             << setw(10) << holdings[i].shares
             << setw(15) << fixed << setprecision(2)
             << holdings[i].avgCostBasis
             << setw(15) << holdings[i].currentPrice
             << setw(15) << ret
             << endl;
    }

    cout << "\nCash Balance: $"
         << fixed << setprecision(2)
         << cashBalance << endl;

    cout << "Total Portfolio Value: $"
         << getTotalValue() << endl;
}

void Portfolio::sortHoldingsByReturn() {

    sort(holdings.begin(), holdings.end(),

        [](const Position& a, const Position& b) {

            double retA =
                ((a.currentPrice - a.avgCostBasis)
                 / a.avgCostBasis);

            double retB =
                ((b.currentPrice - b.avgCostBasis)
                 / b.avgCostBasis);

            return retA > retB;
        });
}

void Portfolio::sortHoldingsByTicker() {

    sort(holdings.begin(), holdings.end(),

        [](const Position& a, const Position& b) {
            return a.ticker < b.ticker;
        });
}

void Portfolio::printTradeHistory() const {
    tradeHistory.printAll();
}

void Portfolio::printPendingOrders() const {
    pendingOrders.printAll();
}

double Portfolio::getCashBalance() const {
    return cashBalance;
}