// MomentumStrategy.cpp
// Implements momentum-threshold signal generation and backtesting.

#include "../include/MomentumStrategy.h"

#include <sstream>
#include <iomanip>

MomentumStrategy::MomentumStrategy(
    double momentumThreshold
) {

    this->momentumThreshold =
        momentumThreshold;
}

MomentumStrategy::~MomentumStrategy() {
}

SimResult MomentumStrategy::backtest(
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

    vector<double> closes;
    vector<string> dates;

    // Convert linked list into arrays for indexing
    for (PriceHistory::Iterator it = history->begin();
         it != history->end();
         ++it) {

        PriceNode& node = *it;

        int year =
            CSVParser::extractYear(node.date);

        if (year < startYear ||
            year > endYear) {
            continue;
        }

        closes.push_back(node.close);
        dates.push_back(node.date);
    }

    if (closes.size() < 130) {
        return result;
    }

    double cash = 0.0;
    double sharesOwned = 0.0;

    bool invested = false;

    vector<double> portfolioValues;

    int lastMonth = -1;
    int lastYear = -1;

    double latestPrice = closes.back();

    for (size_t i = 126;
         i < closes.size();
         i++) {

        string currentDate = dates[i];

        int year =
            CSVParser::extractYear(currentDate);

        int month =
            CSVParser::extractMonth(currentDate);

        bool newMonth =
            (month != lastMonth ||
             year != lastYear);

        // Add monthly contribution
        if (newMonth) {

            cash += monthlyCapital;

            lastMonth = month;
            lastYear = year;
        }

        // Approx 6-month momentum
        double momentum =
            ((closes[i] - closes[i - 126])
            / closes[i - 126]) * 100.0;

        // BUY
        if (momentum > momentumThreshold &&
            !invested) {

            double sharesBought =
                cash / closes[i];

            sharesOwned += sharesBought;

            result.totalInvested += cash;

            cash = 0.0;

            invested = true;

            result.totalTrades++;
        }

        // SELL
        else if (momentum < 0.0 &&
                 invested) {

            cash += sharesOwned * closes[i];

            sharesOwned = 0.0;

            invested = false;

            result.totalTrades++;
        }

        double currentValue =
            cash + sharesOwned * closes[i];

        portfolioValues.push_back(currentValue);
    }

    result.finalValue =
        cash + sharesOwned * latestPrice;

    if (result.totalInvested > 0.0) {

        result.totalReturn =
            ((result.finalValue
            - result.totalInvested)
            / result.totalInvested) * 100.0;
    }

    result.cagr =
        calculateCAGR(
            result.totalInvested,
            result.finalValue,
            endYear - startYear
        );

    result.maxDrawdown =
        calculateMaxDrawdown(
            portfolioValues
        );

    return result;
}

string MomentumStrategy::getName() const {

    stringstream ss;

    ss << "6-Month Momentum (Threshold="
       << fixed << setprecision(2)
       << momentumThreshold
       << "%)";

    return ss.str();
}