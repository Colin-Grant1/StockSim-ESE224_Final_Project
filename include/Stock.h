#ifndef STOCK_H
#define STOCK_H

#include <iostream>
#include <string>
#include "FinancialAsset.h"
#include "PriceHistory.h"
#include "CSVParser.h"

using namespace std;

class Stock : public FinancialAsset {
private:
    PriceHistory* history;

public:
    Stock(const string& ticker, const string& name);
    virtual ~Stock();

    bool loadFromCSV(const string& filename);

    PriceHistory* getHistory() const;
    double getPriceOnDate(const string& date) const;

    virtual double calculateAnnualReturn(int year) const override;
    virtual void printSummary() const override;
    virtual string getType() const override;
};

#endif