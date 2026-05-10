#include "../include/CSVParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>

PriceHistory* CSVParser::loadHistory(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return nullptr;
    }

    PriceHistory* history = new PriceHistory();

    string line;

    // Read header row
    if (!getline(file, line)) {
        cerr << "Error: Empty file " << filename << endl;
        file.close();
        delete history;
        return nullptr;
    }

    // Split header into fixed array
    string headers[10];
    int headerCount = 0;

    stringstream headerStream(line);
    string cell;

    while (getline(headerStream, cell, ',') && headerCount < 10) {
        headers[headerCount++] = cell;
    }

    // Dynamically locate required columns
    int dateIdx   = findColumnIndex(headers, headerCount, "Date");
    int openIdx   = findColumnIndex(headers, headerCount, "Open");
    int highIdx   = findColumnIndex(headers, headerCount, "High");
    int lowIdx    = findColumnIndex(headers, headerCount, "Low");
    int closeIdx  = findColumnIndex(headers, headerCount, "Close");
    int volumeIdx = findColumnIndex(headers, headerCount, "Volume");

    if (dateIdx == -1 || openIdx == -1 || highIdx == -1 ||
        lowIdx == -1 || closeIdx == -1 || volumeIdx == -1) {
        cerr << "Error: Missing required columns in " << filename << endl;
        file.close();
        delete history;
        return nullptr;
    }

    // Read data rows
    while (getline(file, line)) {
        if (line.empty()) continue;

        string values[10];
        int valueCount = 0;

        stringstream rowStream(line);

        while (getline(rowStream, cell, ',') && valueCount < 10) {
            values[valueCount++] = cell;
        }

        // Ensure enough columns
        if (valueCount <= volumeIdx) continue;

        try {
            string date = values[dateIdx];

            if (!dateInRange(date)) continue;

            // Skip malformed / null values
            if (values[openIdx] == "null" || values[highIdx] == "null" ||
                values[lowIdx] == "null"  || values[closeIdx] == "null" ||
                values[volumeIdx] == "null") {
                continue;
            }

            double open   = stod(values[openIdx]);
            double high   = stod(values[highIdx]);
            double low    = stod(values[lowIdx]);
            double close  = stod(values[closeIdx]);
            long volume   = stol(values[volumeIdx]);

            history->append(date, open, high, low, close, volume);
        }
        catch (...) {
            // Skip malformed row silently
            continue;
        }
    }

    file.close();
    return history;
}

bool CSVParser::dateInRange(const string& date, const string& start, const string& end) {
    return (date >= start && date <= end);
}

int CSVParser::extractYear(const string& date) {
    if (date.length() < 4) return -1;
    return stoi(date.substr(0, 4));
}

int CSVParser::extractMonth(const string& date) {
    if (date.length() < 7) return -1;
    return stoi(date.substr(5, 2));
}

int CSVParser::findColumnIndex(const string headers[], int size, const string& target) {
    for (int i = 0; i < size; i++) {
        if (headers[i] == target) {
            return i;
        }
    }
    return -1;
}