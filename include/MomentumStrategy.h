// MomentumStrategy.h
// Declares the momentum-based strategy implementation.

#ifndef MOMENTUMSTRATEGY_H
#define MOMENTUMSTRATEGY_H

#include <iostream>
#include <vector>
#include <string>

#include "TradingStrategy.h"
#include "CSVParser.h"

using namespace std;

class MomentumStrategy : public TradingStrategy {
private:
    double momentumThreshold;

public:
    MomentumStrategy(
        double momentumThreshold = 0.0
    );

    virtual ~MomentumStrategy();

    virtual SimResult backtest(
        PriceHistory* history,
        double monthlyCapital,
        int startYear,
        int endYear
    ) override;

    virtual string getName() const override;
};

#endif