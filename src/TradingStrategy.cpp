// TradingStrategy.cpp
// Implements shared strategy utilities for performance metrics and output.

#include "../include/TradingStrategy.h"

#include <iomanip>
#include <cmath>

TradingStrategy::~TradingStrategy() {
    // Virtual destructor
}

void TradingStrategy::printResult(
    const SimResult& result
) const {

    cout << "\n=====================================\n";
    cout << "Strategy: " << result.strategyName << endl;
    cout << "=====================================\n";

    cout << fixed << setprecision(2);

    cout << "Final Portfolio Value: $"
         << result.finalValue << endl;

    cout << "Total Invested: $"
         << result.totalInvested << endl;

    cout << "Total Return: "
         << result.totalReturn << "%\n";

    cout << "CAGR: "
         << result.cagr << "%\n";

    cout << "Max Drawdown: "
         << result.maxDrawdown << "%\n";

    cout << "Total Trades: "
         << result.totalTrades << endl;
}

double TradingStrategy::calculateCAGR(
    double startVal,
    double endVal,
    int years
) const {

    if (startVal <= 0.0 ||
        endVal <= 0.0 ||
        years <= 0) {
        return 0.0;
    }

    return
        (pow(endVal / startVal,
        1.0 / years) - 1.0) * 100.0;
}

double TradingStrategy::calculateMaxDrawdown(
    const vector<double>& portfolioValues
) const {

    if (portfolioValues.empty()) {
        return 0.0;
    }

    double peak = portfolioValues[0];
    double maxDrawdown = 0.0;

    for (size_t i = 0;
         i < portfolioValues.size();
         i++) {

        if (portfolioValues[i] > peak) {
            peak = portfolioValues[i];
        }

        double drawdown =
            ((peak - portfolioValues[i])
            / peak) * 100.0;

        if (drawdown > maxDrawdown) {
            maxDrawdown = drawdown;
        }
    }

    return maxDrawdown;
}