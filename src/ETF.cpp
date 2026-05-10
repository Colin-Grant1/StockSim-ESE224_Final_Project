// ETF.cpp
// Implements ETF behavior on top of stock price-history mechanics.

#include "../include/ETF.h"
#include <iomanip>
#include <cmath>

ETF::ETF(const string& ticker, const string& name, double expenseRatio)
    : Stock(ticker, name) {
    this->expenseRatio = expenseRatio;
}

double ETF::getExpenseRatio() const {
    return expenseRatio;
}

void ETF::setExpenseRatio(double expenseRatio) {
    this->expenseRatio = expenseRatio;
}

void ETF::printSummary() const {
    PriceHistory* history = getHistory();

    if (history == nullptr || history->getSize() == 0) {
        cout << "No data loaded.\n";
        return;
    }

    PriceNode* first = history->getHead();
    PriceNode* last = history->getTail();

    double totalReturn = 0.0;
    if (first != nullptr && last != nullptr && first->close != 0.0) {
        totalReturn = ((last->close - first->close) / first->close) * 100.0;
    }

    // Approx 10-year CAGR using last 10 years of filtered dataset
    double cagr10 = 0.0;
    PriceNode* tenYearStart = nullptr;

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        int year = CSVParser::extractYear(node.date);

        if (year >= 2010) {
            tenYearStart = &node;
            break;
        }
    }

    if (tenYearStart != nullptr && last->close > 0.0 && tenYearStart->close > 0.0) {
        cagr10 = (pow(last->close / tenYearStart->close, 1.0 / 10.0) - 1.0) * 100.0;
    }

    cout << "===== ETF Summary =====\n";
    cout << "Type: " << getType() << endl;
    cout << "Ticker: " << getTicker() << endl;
    cout << "Name: " << getName() << endl;
    cout << "Sector: " << getSector() << endl;
    cout << "Trading Days: " << history->getSize() << endl;
    cout << "Date Range: " << first->date << " to " << last->date << endl;

    cout << fixed << setprecision(2);
    cout << "Start Price: $" << first->close << endl;
    cout << "End Price: $" << last->close << endl;
    cout << "Total Return: " << totalReturn << "%\n";
    cout << "Expense Ratio: " << expenseRatio << "%\n";
    cout << "10-Year CAGR: " << cagr10 << "%\n";
}

string ETF::getType() const {
    return "ETF";
}