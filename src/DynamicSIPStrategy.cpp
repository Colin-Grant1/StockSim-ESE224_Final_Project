// DynamicSIPStrategy.cpp
// Implements dynamic SIP backtest rules and reporting.

#include "../include/DynamicSIPStrategy.h"

#include <sstream>
#include <iomanip>

DynamicSIPStrategy::DynamicSIPStrategy(
    int maDays,
    double dipThreshold,
    double expensiveThreshold
) {

    this->movingAverageDays = maDays;

    this->dipThreshold = dipThreshold;

    this->expensiveThreshold = expensiveThreshold;
}

DynamicSIPStrategy::~DynamicSIPStrategy() {
}

SimResult DynamicSIPStrategy::backtest(
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

    CircularQueue maQueue(movingAverageDays);

    vector<double> portfolioValues;

    double sharesOwned = 0.0;

    double reserveCash = 0.0;

    int lastMonth = -1;
    int lastYear = -1;

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

        maQueue.enqueue(node.close);

        double ma =
            maQueue.getAverage();

        bool newMonth =
            (month != lastMonth ||
             year != lastYear);

        if (newMonth &&
            maQueue.getCount() >= movingAverageDays) {

            reserveCash += monthlyCapital;

            double investAmount =
                monthlyCapital;

            double ratio =
                node.close / ma;

            // BUY MORE ON DIPS
            if (ratio < dipThreshold) {

                double extra =
                    monthlyCapital;

                if (reserveCash >= extra) {

                    investAmount =
                        monthlyCapital * 2.0;
                }
            }

            // BUY LESS WHEN EXPENSIVE
            else if (ratio > expensiveThreshold) {

                investAmount =
                    monthlyCapital * 0.5;
            }

            // Prevent overspending
            if (investAmount > reserveCash) {
                investAmount = reserveCash;
            }

            double sharesBought =
                investAmount / node.close;

            sharesOwned += sharesBought;

            reserveCash -= investAmount;

            result.totalInvested +=
                investAmount;

            result.totalTrades++;

            lastMonth = month;
            lastYear = year;
        }

        double currentValue =
            sharesOwned * node.close
            + reserveCash;

        portfolioValues.push_back(currentValue);
    }

    result.finalValue =
        sharesOwned * latestPrice
        + reserveCash;

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

string DynamicSIPStrategy::getName() const {

    stringstream ss;

    ss << "Dynamic SIP (MA"
       << movingAverageDays
       << ", Dip="
       << fixed << setprecision(2)
       << dipThreshold
       << ", Exp="
       << expensiveThreshold
       << ")";

    return ss.str();
}