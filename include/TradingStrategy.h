// TradingStrategy.h
// Declares shared backtesting result types and strategy base behavior.

#ifndef TRADINGSTRATEGY_H
#define TRADINGSTRATEGY_H

#include <iostream>
#include <vector>
#include <string>

#include "PriceHistory.h"

using namespace std;

struct SimResult {
    string strategyName;

    double finalValue;
    double totalInvested;
    double totalReturn;

    double cagr;
    double maxDrawdown;

    int totalTrades;
};

class TradingStrategy {
public:
    virtual ~TradingStrategy();

    virtual SimResult backtest(
        PriceHistory* history,
        double monthlyCapital,
        int startYear,
        int endYear
    ) = 0;

    virtual string getName() const = 0;

    void printResult(const SimResult& result) const;

protected:
    double calculateCAGR(
        double startVal,
        double endVal,
        int years
    ) const;

    double calculateMaxDrawdown(
        const vector<double>& portfolioValues
    ) const;
};

#endif