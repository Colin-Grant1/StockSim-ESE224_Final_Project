#ifndef FINANCIALASSET_H
#define FINANCIALASSET_H

#include <iostream>
#include <string>

using namespace std;

class FinancialAsset {
protected:
    string ticker;
    string name;
    string sector;

public:
    FinancialAsset(const string& ticker, const string& name);
    virtual ~FinancialAsset();

    // Pure virtual interface
    virtual void printSummary() const = 0;
    virtual double calculateAnnualReturn(int year) const = 0;
    virtual string getType() const = 0;

    // Shared getters/setters
    string getTicker() const;
    string getName() const;
    void setSector(const string& sector);
    string getSector() const;
};

#endif