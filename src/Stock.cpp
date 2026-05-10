#include "../include/Stock.h"
#include <iomanip>

Stock::Stock(const string& ticker, const string& name)
    : FinancialAsset(ticker, name) {
    history = nullptr;
}

Stock::~Stock() {
    if (history != nullptr) {
        delete history;
        history = nullptr;
    }
}

bool Stock::loadFromCSV(const string& filename) {
    if (history != nullptr) {
        delete history;
        history = nullptr;
    }

    history = CSVParser::loadHistory(filename);
    return (history != nullptr);
}

PriceHistory* Stock::getHistory() const {
    return history;
}

double Stock::getPriceOnDate(const string& date) const {
    if (history == nullptr) return -1.0;

    PriceNode* node = history->findByDate(date);
    if (node == nullptr) return -1.0;

    return node->close;
}

double Stock::calculateAnnualReturn(int year) const {
    if (history == nullptr || history->getSize() == 0) return 0.0;

    PriceNode* startNode = nullptr;
    PriceNode* endNode = nullptr;

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        int nodeYear = CSVParser::extractYear(node.date);

        if (nodeYear == year && startNode == nullptr) {
            startNode = &node;
        }

        if (nodeYear == year) {
            endNode = &node;
        }
    }

    if (startNode == nullptr || endNode == nullptr) return 0.0;
    if (startNode->close == 0.0) return 0.0;

    return ((endNode->close - startNode->close) / startNode->close) * 100.0;
}

void Stock::printSummary() const {
    if (history == nullptr || history->getSize() == 0) {
        cout << "No data loaded for " << ticker << endl;
        return;
    }

    PriceNode* first = history->getHead();
    PriceNode* last = history->getTail();

    double totalReturn = 0.0;
    if (first != nullptr && last != nullptr && first->close != 0.0) {
        totalReturn = ((last->close - first->close) / first->close) * 100.0;
    }

    cout << "===== Stock Summary =====\n";
    cout << "Type: " << getType() << endl;
    cout << "Ticker: " << ticker << endl;
    cout << "Name: " << name << endl;
    cout << "Sector: " << sector << endl;
    cout << "Trading Days: " << history->getSize() << endl;
    cout << "Date Range: " << first->date << " to " << last->date << endl;
    cout << fixed << setprecision(2);
    cout << "Start Price: $" << first->close << endl;
    cout << "End Price: $" << last->close << endl;
    cout << "Total Return: " << totalReturn << "%\n";
}

string Stock::getType() const {
    return "Stock";
}