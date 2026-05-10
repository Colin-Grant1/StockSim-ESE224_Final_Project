// FixedSIPStrategy.h
// Declares the fixed SIP strategy implementation.

#ifndef FIXEDSIPSTRATEGY_H
#define FIXEDSIPSTRATEGY_H

#include <iostream>
#include <vector>
#include <string>

#include "TradingStrategy.h"
#include "CSVParser.h"

using namespace std;

class FixedSIPStrategy : public TradingStrategy {
public:
    FixedSIPStrategy();

    virtual ~FixedSIPStrategy();

    virtual SimResult backtest(
        PriceHistory* history,
        double monthlyCapital,
        int startYear,
        int endYear
    ) override;

    virtual string getName() const override;
};

#endif