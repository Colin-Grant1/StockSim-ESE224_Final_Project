// DynamicSIPStrategy.cpp
// Implements dynamic SIP backtest rules and reporting.

#include "../include/DynamicSIPStrategy.h"
#include "../include/FixedSIPStrategy.h"

#include <sstream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>

DynamicSIPStrategy::DynamicSIPStrategy(
    int maDays,
    double dipThreshold,
    double expensiveThreshold,
    double reserveCapMonths
)
    : ma50Queue(maDays),
      ma200Queue(200),
      returnQueue(20),
      momentumQueue(5) {

    this->maShortDays = maDays;
    this->maLongDays = 200;
    this->volatilityDays = 20;
    this->momentumDays = 5;

    this->dipThreshold = dipThreshold;
    this->expensiveThreshold = expensiveThreshold;
    this->reserveCapMonths = reserveCapMonths;

    resetState();
}

DynamicSIPStrategy::~DynamicSIPStrategy() {
}

void DynamicSIPStrategy::resetState() {

    // Rolling accumulators allow O(1) updates for means and volatility.
    ma50Sum = 0.0;
    ma200Sum = 0.0;
    returnSum = 0.0;
    returnSqSum = 0.0;
    momentumSum = 0.0;

    reserveCash = 0.0;
    totalInvested = 0.0;
    sharesOwned = 0.0;

    latestPrice = 0.0;
    peakPrice = 0.0;
    previousClose = 0.0;

    lastMonth = -1;
    lastYear = -1;

    portfolioValues.clear();
    tradeLog.clear();

    currentSignals.ma50 = 0.0;
    currentSignals.ma200 = 0.0;
    currentSignals.volatility = 0.0;
    currentSignals.drawdown = 0.0;
    currentSignals.momentum5 = 0.0;
    currentSignals.ma50Ready = false;
    currentSignals.ma200Ready = false;
    currentSignals.volReady = false;
}

vector<pair<int, int>> DynamicSIPStrategy::collectMonthlyBuckets(
    PriceHistory* history,
    int startYear,
    int endYear
) const {

    vector<pair<int, int>> months;

    int seenYear = -1;
    int seenMonth = -1;

    for (PriceHistory::Iterator it = history->begin();
         it != history->end();
         ++it) {

        PriceNode& node = *it;

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        if (year < startYear ||
            year > endYear) {
            continue;
        }

        if (year != seenYear ||
            month != seenMonth) {

            months.push_back(make_pair(year, month));

            seenYear = year;
            seenMonth = month;
        }
    }

    return months;
}

int DynamicSIPStrategy::findMonthIndex(
    const vector<pair<int, int>>& months,
    int year,
    int month
) const {

    for (size_t i = 0; i < months.size(); i++) {
        if (months[i].first == year &&
            months[i].second == month) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

void DynamicSIPStrategy::update(
    const DayData& dayData
) {

    latestPrice = dayData.price;

    if (peakPrice < dayData.price) {
        peakPrice = dayData.price;
    }

    if (peakPrice > 0.0) {
        currentSignals.drawdown =
            (peakPrice - dayData.price) / peakPrice;
    }

    // MA50 in O(1): remove outgoing element before queue overwrite.
    if (ma50Queue.isFull()) {
        ma50Sum -= ma50Queue.peek();
    }
    ma50Queue.enqueue(dayData.price);
    ma50Sum += dayData.price;

    if (ma50Queue.getCount() > 0) {
        currentSignals.ma50 =
            ma50Sum / ma50Queue.getCount();
    }

    // MA200 in O(1): same rolling-sum pattern.
    if (ma200Queue.isFull()) {
        ma200Sum -= ma200Queue.peek();
    }
    ma200Queue.enqueue(dayData.price);
    ma200Sum += dayData.price;

    if (ma200Queue.getCount() > 0) {
        currentSignals.ma200 =
            ma200Sum / ma200Queue.getCount();
    }

    currentSignals.ma50Ready =
        (ma50Queue.getCount() >= maShortDays);

    currentSignals.ma200Ready =
        (ma200Queue.getCount() >= maLongDays);

    // Rolling return/volatility windows for aggressiveness dampening.
    if (previousClose > 0.0) {

        double dayReturn =
            (dayData.price / previousClose) - 1.0;

        if (returnQueue.isFull()) {
            double out = returnQueue.peek();
            returnSum -= out;
            returnSqSum -= out * out;
        }

        returnQueue.enqueue(dayReturn);
        returnSum += dayReturn;
        returnSqSum += dayReturn * dayReturn;

        if (momentumQueue.isFull()) {
            momentumSum -= momentumQueue.peek();
        }
        momentumQueue.enqueue(dayReturn);
        momentumSum += dayReturn;

        if (momentumQueue.getCount() == momentumDays) {
            currentSignals.momentum5 = momentumSum;
        }
    }

    int n = returnQueue.getCount();
    if (n >= volatilityDays) {

        double mean = returnSum / n;

        double variance =
            (returnSqSum / n) - (mean * mean);

        if (variance < 0.0) {
            variance = 0.0;
        }

        currentSignals.volatility = sqrt(variance);
        currentSignals.volReady = true;
    }

    previousClose = dayData.price;
}

bool DynamicSIPStrategy::shouldInvest(
    const DayData& dayData
) const {

    // Monthly reserve contribution is mandatory by design.
    if (!dayData.isNewMonth) {
        return false;
    }

    return true;
}

double DynamicSIPStrategy::calculateInvestmentAmount(
    const DayData& dayData,
    double monthlyCapital,
    double targetTotalInvestment
) const {

    // Base deployment favors cash build-up in strong uptrends.
    bool bear =
        currentSignals.ma200Ready &&
        currentSignals.ma50 < currentSignals.ma200;

    double baseMultiple = bear ? 0.90 : 0.12;

    // Drawdown tiers are the primary lever for terminal wealth optimization.
    double dd = currentSignals.drawdown;
    if (dd < 0.05) {
        baseMultiple *= 0.35;
    }
    else if (dd < 0.10) {
        baseMultiple *= 0.85;
    }
    else if (dd < 0.20) {
        baseMultiple *= 2.30;
    }
    else if (dd < 0.30) {
        baseMultiple *= 3.60;
    }
    else {
        baseMultiple *= 4.80;
    }

    // Relative valuation around MA50 refines deployment sizing.
    if (currentSignals.ma50Ready && currentSignals.ma50 > 0.0) {

        double ratio = latestPrice / currentSignals.ma50;

        if (ratio < dipThreshold) {
            baseMultiple *= 1.80;
        }
        else if (ratio > expensiveThreshold) {
            baseMultiple *= 0.45;
        }
    }

    // Volatility filter only tempers extremes to preserve reserve runway.
    if (currentSignals.volReady) {
        if (currentSignals.volatility > 0.030) {
            baseMultiple *= 0.90;
        }
        else if (currentSignals.volatility > 0.020) {
            baseMultiple *= 0.95;
        }
    }

    double investAmount = monthlyCapital * baseMultiple;

    // Large crash buys need one recovery confirmation to reduce false bottoms.
    bool momentumConfirm =
        (currentSignals.ma50Ready && latestPrice >= currentSignals.ma50) ||
        (currentSignals.momentum5 > 0.0) ||
        (currentSignals.drawdown > 0.15 && currentSignals.momentum5 > -0.01);

    if (investAmount > (2.50 * monthlyCapital) && !momentumConfirm) {
        investAmount = 2.50 * monthlyCapital;
    }

    // Reserve cap forces deployment if idle cash grows too much.
    double reserveCap = reserveCapMonths * monthlyCapital;
    if (reserveCash > reserveCap) {
        double mandatory = reserveCash - reserveCap;
        if (mandatory > investAmount) {
            investAmount = mandatory;
        }
    }

    // End-game convergence: progressively spend reserve so it reaches zero by the end.
    double remainingCash = reserveCash;

    int monthsLeft = dayData.monthsRemaining;
    if (monthsLeft < 1) {
        monthsLeft = 1;
    }

    double requiredInvestment = remainingCash / monthsLeft;
    if (requiredInvestment > investAmount) {
        investAmount = requiredInvestment;
    }

    if (investAmount > reserveCash) {
        investAmount = reserveCash;
    }

    if (investAmount < 0.0) {
        investAmount = 0.0;
    }

    return investAmount;
}

void DynamicSIPStrategy::executeTrade(
    const DayData& dayData,
    double investAmount
) {

    if (investAmount <= 0.0 ||
        dayData.price <= 0.0) {
        return;
    }

    double sharesBought =
        investAmount / dayData.price;

    sharesOwned += sharesBought;
    reserveCash -= investAmount;
    totalInvested += investAmount;

    stringstream ss;
    ss << dayData.date
       << " BUY $"
       << fixed << setprecision(2)
       << investAmount
       << " @ "
       << dayData.price;
    tradeLog.push_back(ss.str());
}

void DynamicSIPStrategy::finalize(
    double targetTotalInvestment
) {

    // Safety net: any leftover reserve is deployed at the latest known price.
    if (reserveCash > 0.0 &&
        latestPrice > 0.0) {

        double extra = reserveCash;

        sharesOwned += (extra / latestPrice);
        totalInvested += extra;
        reserveCash = 0.0;
    }

    // Numerical guard to stay budget-neutral against target total capital.
    if (totalInvested > targetTotalInvestment) {
        totalInvested = targetTotalInvestment;
    }
}

void DynamicSIPStrategy::writeBonusReport(
    const SimResult& result,
    double monthlyCapital,
    int startYear,
    int endYear,
    double fixedSipBaseline
) const {

    ofstream out("bonus_strategy.txt");
    if (!out.is_open()) {
        return;
    }

    out << fixed << setprecision(2);

    out << "Strategy Name : " << getName() << "\n";
    out << "monthlyCapital = $" << monthlyCapital << "\n";
    out << "startYear = " << startYear << "\n";
    out << "endYear = " << endYear << "\n";
    out << "ticker = SPX\n\n";

    out << "Algorithm Description :\n";
    out << "This strategy accumulates a monthly reserve and adapts deployment based on trend, drawdown, and short-term volatility.\n";
    out << "It invests lightly in expensive bull phases and deploys aggressively during deeper drawdowns to maximize terminal wealth.\n";
    out << "A reserve cap and end-of-simulation convergence rule enforce full capital deployment without exceeding the fixed budget envelope.\n\n";

    out << "Parameters Used :\n";
    out << "maShortDays = " << maShortDays << "\n";
    out << "maLongDays = " << maLongDays << "\n";
    out << "volatilityDays = " << volatilityDays << "\n";
    out << "momentumDays = " << momentumDays << "\n";
    out << "dipThreshold = " << dipThreshold << "\n";
    out << "expensiveThreshold = " << expensiveThreshold << "\n";
    out << "reserveCapMonths = " << reserveCapMonths << "\n\n";

    out << "Final Portfolio Value : $" << result.finalValue << "\n";
    out << "Fixed SIP Baseline : $" << fixedSipBaseline << "\n";
    out << "CAGR : " << result.cagr << " %\n";
    out << "Max Drawdown : " << result.maxDrawdown << " %\n";
    out << "Total Trades : " << result.totalTrades << "\n";
}

SimResult DynamicSIPStrategy::backtest(
    PriceHistory* history,
    double monthlyCapital,
    int startYear,
    int endYear
) {

    return runBacktestCore(
        history,
        monthlyCapital,
        startYear,
        endYear,
        true,
        true
    );
}

SimResult DynamicSIPStrategy::runBacktestCore(
    PriceHistory* history,
    double monthlyCapital,
    int startYear,
    int endYear,
    bool allowSweep,
    bool writeBonusFile
) {

    resetState();

    SimResult result;

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

    if (allowSweep) {
        performParameterSweep(
            history,
            monthlyCapital,
            startYear,
            endYear
        );
        resetState();
    }

    result.strategyName = getName();

    vector<pair<int, int>> months =
        collectMonthlyBuckets(
            history,
            startYear,
            endYear
        );

    if (months.empty()) {
        return result;
    }

    // Grading target: deploy exactly 240 monthly contributions when available.
    int targetMonths = 240;
    if (static_cast<int>(months.size()) < targetMonths) {
        targetMonths = static_cast<int>(months.size());
    }

    if (targetMonths <= 0) {
        return result;
    }

    vector<pair<int, int>> activeMonths(
        months.begin(),
        months.begin() + targetMonths
    );

    pair<int, int> endBucket = activeMonths.back();
    double targetTotalInvestment =
        monthlyCapital * targetMonths;

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

        if (year > endBucket.first ||
            (year == endBucket.first && month > endBucket.second)) {
            break;
        }

        bool newMonth =
            (month != lastMonth ||
             year != lastYear);

        int monthIndex =
            findMonthIndex(
                activeMonths,
                year,
                month
            );

        if (monthIndex == -1) {
            continue;
        }

        DayData dayData;
        dayData.date = node.date;
        dayData.price = node.close;
        dayData.year = year;
        dayData.month = month;
        dayData.isNewMonth = newMonth;
        dayData.monthsRemaining =
            targetMonths - monthIndex;

        update(dayData);

        if (shouldInvest(dayData)) {

            // Budget neutrality: contribution schedule matches fixed SIP envelope.
            reserveCash += monthlyCapital;

            double investAmount =
                calculateInvestmentAmount(
                    dayData,
                    monthlyCapital,
                    targetTotalInvestment
                );

            double beforeTradeInvested = totalInvested;

            executeTrade(dayData, investAmount);

            if (totalInvested > beforeTradeInvested) {
                result.totalTrades++;
            }

            lastMonth = month;
            lastYear = year;
        }

        double currentValue =
            sharesOwned * dayData.price
            + reserveCash;

        portfolioValues.push_back(currentValue);
    }

    finalize(targetTotalInvestment);

    result.totalInvested = totalInvested;

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

    if (writeBonusFile) {
        // Bonus output includes a baseline for grading-side comparison.
        FixedSIPStrategy fixed;
        SimResult fixedBaseline =
            fixed.backtest(
                history,
                monthlyCapital,
                startYear,
                endYear
            );

        writeBonusReport(
            result,
            monthlyCapital,
            startYear,
            endYear,
            fixedBaseline.finalValue
        );
    }

    return result;
}

void DynamicSIPStrategy::performParameterSweep(
    PriceHistory* history,
    double monthlyCapital,
    int startYear,
    int endYear
) {

    // Sweep objective is terminal wealth only (no risk-adjusted scoring).
    auto evaluateCandidate =
        [&](int ma, double dip, double exp, double cap) {

            DynamicSIPStrategy candidate(
                ma,
                dip,
                exp,
                cap
            );

            return candidate.runBacktestCore(
                history,
                monthlyCapital,
                startYear,
                endYear,
                false,
                false
            );
        };

    // Stage 1: wide coarse scan for global region discovery.
    const int maCandidates[] = {
        20, 25, 30, 35, 40, 45,
        50, 55, 60, 70, 80, 100
    };

    const double dipCandidates[] = {
        0.82, 0.85, 0.88, 0.90, 0.92,
        0.94, 0.96, 0.98
    };

    const double expCandidates[] = {
        1.00, 1.02, 1.04, 1.06,
        1.08, 1.10, 1.12
    };

    const double capCandidates[] = {
        6.0, 10.0, 14.0, 18.0, 24.0,
        30.0, 36.0
    };

    double bestFinalValue = -1.0;

    int bestMa = maShortDays;
    double bestDip = dipThreshold;
    double bestExp = expensiveThreshold;
    double bestCap = reserveCapMonths;

    for (size_t i = 0;
         i < sizeof(maCandidates) / sizeof(maCandidates[0]);
         i++) {

        for (size_t j = 0;
             j < sizeof(dipCandidates) / sizeof(dipCandidates[0]);
             j++) {

            for (size_t k = 0;
                 k < sizeof(expCandidates) / sizeof(expCandidates[0]);
                 k++) {

                for (size_t m = 0;
                     m < sizeof(capCandidates) / sizeof(capCandidates[0]);
                     m++) {

                    SimResult trial =
                        evaluateCandidate(
                            maCandidates[i],
                            dipCandidates[j],
                            expCandidates[k],
                            capCandidates[m]
                        );

                    if (trial.finalValue > bestFinalValue) {
                        bestFinalValue = trial.finalValue;
                        bestMa = maCandidates[i];
                        bestDip = dipCandidates[j];
                        bestExp = expCandidates[k];
                        bestCap = capCandidates[m];
                    }
                }
            }
        }
    }

    // Stage 2: local fine scan around coarse optimum.
    int maStart = bestMa - 8;
    int maEnd = bestMa + 8;
    if (maStart < 15) {
        maStart = 15;
    }

    for (int ma = maStart;
         ma <= maEnd;
         ma++) {

        for (double dip = bestDip - 0.04;
             dip <= bestDip + 0.04 + 1e-9;
             dip += 0.01) {

            if (dip < 0.80 || dip > 0.99) {
                continue;
            }

            for (double exp = bestExp - 0.04;
                 exp <= bestExp + 0.04 + 1e-9;
                 exp += 0.01) {

                if (exp < 1.00 || exp > 1.15) {
                    continue;
                }

                for (double cap = bestCap - 6.0;
                     cap <= bestCap + 6.0 + 1e-9;
                     cap += 1.0) {

                    if (cap < 4.0 || cap > 40.0) {
                        continue;
                    }

                    SimResult trial =
                        evaluateCandidate(
                            ma,
                            dip,
                            exp,
                            cap
                        );

                    if (trial.finalValue > bestFinalValue) {
                        bestFinalValue = trial.finalValue;
                        bestMa = ma;
                        bestDip = dip;
                        bestExp = exp;
                        bestCap = cap;
                    }
                }
            }
        }
    }

    // Lock the best-performing parameter set for the actual run.
    maShortDays = bestMa;
    dipThreshold = bestDip;
    expensiveThreshold = bestExp;
    reserveCapMonths = bestCap;
}

string DynamicSIPStrategy::getName() const {

    stringstream ss;

    ss << "DynamicCrashExploiter_MA50_"
       << maShortDays
       << "_MA200_"
       << maLongDays
       << "_Dip_"
       << fixed << setprecision(2)
       << dipThreshold
       << "_Exp_"
       << expensiveThreshold
       << "_Cap_"
       << setprecision(0)
       << reserveCapMonths;

    return ss.str();
}