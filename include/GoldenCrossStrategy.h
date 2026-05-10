#ifndef GOLDENCROSSSTRATEGY_H
#define GOLDENCROSSSTRATEGY_H

#include <iostream>
#include <vector>
#include <string>

#include "TradingStrategy.h"
#include "CircularQueue.h"
#include "CSVParser.h"

using namespace std;

class GoldenCrossStrategy : public TradingStrategy {
private:
    int shortWindow;
    int longWindow;

public:
    GoldenCrossStrategy(
        int shortWindow = 50,
        int longWindow = 200
    );

    virtual ~GoldenCrossStrategy();

    virtual SimResult backtest(
        PriceHistory* history,
        double monthlyCapital,
        int startYear,
        int endYear
    ) override;

    virtual string getName() const override;
};

#endif