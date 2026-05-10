// FixedSIPStrategy.cpp
// Implements fixed SIP contribution backtesting.

#include "../include/FixedSIPStrategy.h"

#include <iomanip>

FixedSIPStrategy::FixedSIPStrategy() {
}

FixedSIPStrategy::~FixedSIPStrategy() {
}

SimResult FixedSIPStrategy::backtest(
    PriceHistory* history,
    double monthlyCapital,
    int startYear,
    int endYear
) {

    SimResult result;

    result.strategyName = getName();

    result.finalValue = 0.0;
    result.totalInvested = 0.0;
    result.totalReturn = 0.0;
    result.cagr = 0.0;
    result.maxDrawdown = 0.0;
    result.totalTrades = 0;

    if (history == nullptr ||
        history->getSize() == 0) {
        return result;
    }

    double sharesOwned = 0.0;

    int lastMonth = -1;
    int lastYear = -1;

    vector<double> portfolioValues;

    double latestPrice = 0.0;

    for (PriceHistory::Iterator it = history->begin();
         it != history->end();
         ++it) {

        PriceNode& node = *it;

        int year =
            CSVParser::extractYear(node.date);

        int month =
            CSVParser::extractMonth(node.date);

        if (year < startYear ||
            year > endYear) {
            continue;
        }

        latestPrice = node.close;

        bool newMonth =
            (month != lastMonth ||
             year != lastYear);

        if (newMonth) {

            double sharesBought =
                monthlyCapital / node.close;

            sharesOwned += sharesBought;

            result.totalInvested +=
                monthlyCapital;

            result.totalTrades++;

            lastMonth = month;
            lastYear = year;
        }

        double currentValue =
            sharesOwned * node.close;

        portfolioValues.push_back(currentValue);
    }

    result.finalValue =
        sharesOwned * latestPrice;

    if (result.totalInvested > 0.0) {

        result.totalReturn =
            ((result.finalValue -
              result.totalInvested)
             / result.totalInvested) * 100.0;
    }

    result.cagr =
        calculateCAGR(
            result.totalInvested,
            result.finalValue,
            endYear - startYear
        );

    result.maxDrawdown =
        calculateMaxDrawdown(portfolioValues);

    return result;
}

string FixedSIPStrategy::getName() const {
    return "Fixed SIP";
}