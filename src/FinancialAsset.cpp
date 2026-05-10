// FinancialAsset.cpp
// Implements core financial-asset fields and shared analytics methods.

#include "../include/FinancialAsset.h"

FinancialAsset::FinancialAsset(const string& ticker, const string& name) {
    this->ticker = ticker;
    this->name = name;
    this->sector = "Unknown";
}

FinancialAsset::~FinancialAsset() {
    // Virtual destructor for polymorphic cleanup
}

string FinancialAsset::getTicker() const {
    return ticker;
}

string FinancialAsset::getName() const {
    return name;
}

void FinancialAsset::setSector(const string& sector) {
    this->sector = sector;
}

string FinancialAsset::getSector() const {
    return sector;
}