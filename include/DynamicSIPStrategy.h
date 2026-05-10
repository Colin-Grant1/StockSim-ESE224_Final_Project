// DynamicSIPStrategy.h
// Declares the dynamic SIP backtesting strategy and its parameters.

#ifndef DYNAMICSIPSTRATEGY_H
#define DYNAMICSIPSTRATEGY_H

#include <iostream>
#include <vector>
#include <string>
#include <utility>

#include "TradingStrategy.h"
#include "CircularQueue.h"
#include "CSVParser.h"

using namespace std;

class DynamicSIPStrategy : public TradingStrategy {
private:
    struct DayData {
        string date;
        double price;
        int year;
        int month;
        bool isNewMonth;
        int monthsRemaining;
    };

    struct SignalState {
        double ma50;
        double ma200;
        double volatility;
        double drawdown;
        double momentum5;
        bool ma50Ready;
        bool ma200Ready;
        bool volReady;
    };

    int maShortDays;
    int maLongDays;
    int volatilityDays;
    int momentumDays;

    double dipThreshold;
    double expensiveThreshold;
    double reserveCapMonths;

    CircularQueue ma50Queue;
    CircularQueue ma200Queue;
    CircularQueue returnQueue;
    CircularQueue momentumQueue;

    double ma50Sum;
    double ma200Sum;
    double returnSum;
    double returnSqSum;
    double momentumSum;

    double reserveCash;
    double totalInvested;
    double sharesOwned;
    double latestPrice;
    double peakPrice;
    double previousClose;

    int lastMonth;
    int lastYear;

    vector<double> portfolioValues;
    vector<string> tradeLog;
    SignalState currentSignals;

    void resetState();
    void update(const DayData& dayData);
    bool shouldInvest(const DayData& dayData) const;
    double calculateInvestmentAmount(
        const DayData& dayData,
        double monthlyCapital,
        double targetTotalInvestment
    ) const;
    void executeTrade(
        const DayData& dayData,
        double investAmount
    );
    void finalize(double targetTotalInvestment);

    vector<pair<int, int>> collectMonthlyBuckets(
        PriceHistory* history,
        int startYear,
        int endYear
    ) const;

    int findMonthIndex(
        const vector<pair<int, int>>& months,
        int year,
        int month
    ) const;

    void writeBonusReport(
        const SimResult& result,
        double monthlyCapital,
        int startYear,
        int endYear,
        double fixedSipBaseline
    ) const;

public:
    DynamicSIPStrategy(
        int maDays = 50,
        double dipThreshold = 0.95,
        double expensiveThreshold = 1.05,
        double reserveCapMonths = 18.0
    );

    virtual ~DynamicSIPStrategy();

    virtual SimResult backtest(
        PriceHistory* history,
        double monthlyCapital,
        int startYear,
        int endYear
    ) override;

    virtual string getName() const override;

private:
    SimResult runBacktestCore(
        PriceHistory* history,
        double monthlyCapital,
        int startYear,
        int endYear,
        bool allowSweep,
        bool writeBonusFile
    );

    void performParameterSweep(
        PriceHistory* history,
        double monthlyCapital,
        int startYear,
        int endYear
    );
};

#endif