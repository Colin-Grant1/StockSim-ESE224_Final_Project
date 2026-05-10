#include "../include/GoldenCrossStrategy.h"

#include <sstream>
#include <iomanip>

GoldenCrossStrategy::GoldenCrossStrategy(
    int shortWindow,
    int longWindow
) {

    this->shortWindow = shortWindow;
    this->longWindow = longWindow;
}

GoldenCrossStrategy::~GoldenCrossStrategy() {
}

SimResult GoldenCrossStrategy::backtest(
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

    CircularQueue shortMA(shortWindow);
    CircularQueue longMA(longWindow);

    vector<double> portfolioValues;

    double cash = 0.0;
    double sharesOwned = 0.0;

    bool inMarket = false;

    double previousShort = 0.0;
    double previousLong = 0.0;

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

        shortMA.enqueue(node.close);
        longMA.enqueue(node.close);

        bool newMonth =
            (month != lastMonth ||
             year != lastYear);

        // Add monthly contribution
        if (newMonth) {

            cash += monthlyCapital;

            lastMonth = month;
            lastYear = year;
        }

        // Need enough data
        if (shortMA.getCount() < shortWindow ||
            longMA.getCount() < longWindow) {

            double currentValue =
                cash + sharesOwned * node.close;

            portfolioValues.push_back(currentValue);

            continue;
        }

        double shortAvg =
            shortMA.getAverage();

        double longAvg =
            longMA.getAverage();

        bool goldenCross =
            (previousShort <= previousLong &&
             shortAvg > longAvg);

        bool deathCross =
            (previousShort >= previousLong &&
             shortAvg < longAvg);

        // BUY SIGNAL
        if (goldenCross && !inMarket) {

            double sharesBought =
                cash / node.close;

            sharesOwned += sharesBought;

            result.totalInvested += cash;

            cash = 0.0;

            inMarket = true;

            result.totalTrades++;
        }

        // SELL SIGNAL
        else if (deathCross && inMarket) {

            cash += sharesOwned * node.close;

            sharesOwned = 0.0;

            inMarket = false;

            result.totalTrades++;
        }

        previousShort = shortAvg;
        previousLong = longAvg;

        double currentValue =
            cash + sharesOwned * node.close;

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

string GoldenCrossStrategy::getName() const {

    stringstream ss;

    ss << "Golden Cross ("
       << shortWindow
       << "/"
       << longWindow
       << " MA)";

    return ss.str();
}