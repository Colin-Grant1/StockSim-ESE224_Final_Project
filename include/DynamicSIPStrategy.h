// DynamicSIPStrategy.h
// Declares the dynamic SIP backtesting strategy and its parameters.

#ifndef DYNAMICSIPSTRATEGY_H
#define DYNAMICSIPSTRATEGY_H

#include <iostream>
#include <vector>
#include <string>

#include "TradingStrategy.h"
#include "CircularQueue.h"
#include "CSVParser.h"

using namespace std;

class DynamicSIPStrategy : public TradingStrategy {
private:
    int movingAverageDays;

    double dipThreshold;
    double expensiveThreshold;

public:
    DynamicSIPStrategy(
        int maDays = 30,
        double dipThreshold = 0.95,
        double expensiveThreshold = 1.05
    );

    virtual ~DynamicSIPStrategy();

    virtual SimResult backtest(
        PriceHistory* history,
        double monthlyCapital,
        int startYear,
        int endYear
    ) override;

    virtual string getName() const override;
};

#endif