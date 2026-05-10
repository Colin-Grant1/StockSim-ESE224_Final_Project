// ETF.h
// Declares the ETF asset type built on top of Stock behavior.

#ifndef ETF_H
#define ETF_H

#include <iostream>
#include <string>
#include "Stock.h"

using namespace std;

class ETF : public Stock {
private:
    double expenseRatio;

public:
    ETF(const string& ticker, const string& name, double expenseRatio = 0.0);

    double getExpenseRatio() const;
    void setExpenseRatio(double expenseRatio);

    virtual void printSummary() const override;
    virtual string getType() const override;
};

#endif