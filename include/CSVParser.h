#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <string>
#include "PriceHistory.h"

using namespace std;

class CSVParser {
public:
    static PriceHistory* loadHistory(const string& filename);

    static bool dateInRange(const string& date,
                            const string& start = "2000-01-01",
                            const string& end   = "2020-01-01");

    static int extractYear(const string& date);
    static int extractMonth(const string& date);

private:
    static int findColumnIndex(const string headers[], int size, const string& target);
};

#endif