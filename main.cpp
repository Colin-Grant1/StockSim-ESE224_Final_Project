#include <iostream>
#include <iomanip>
#include <string>
#include <limits>

#include "include/Stock.h"
#include "include/ETF.h"
#include "include/StockManager.h"
#include "include/Portfolio.h"
#include "include/StockBST.h"

#include "include/FixedSIPStrategy.h"
#include "include/DynamicSIPStrategy.h"
#include "include/GoldenCrossStrategy.h"
#include "include/MomentumStrategy.h"

using namespace std;

void printMenu(
    const string& studentName,
    const string& studentID
) {

    // Centralized menu printer keeps prompts consistent across loop iterations.
    cout << "\n=============================================\n";
    cout << "===== StockSim: Historical Market Analyzer =====\n";
    cout << "=============================================\n";
    cout << "Student: " << studentName
         << " | ID: " << studentID << "\n";

    cout << "[1]  Load stock data from CSV\n";
    cout << "[2]  Display price history\n";
    cout << "[3]  Search by date range\n";
    cout << "[4]  Find stocks by return range (BST)\n";
    cout << "[5]  Insert stock into performance BST\n";
    cout << "[6]  Display BST traversals\n";
    cout << "[7]  Add stock to portfolio\n";
    cout << "[8]  Remove stock from portfolio\n";
    cout << "[9]  Queue pending order\n";
    cout << "[10] Execute next pending order\n";
    cout << "[11] Undo last trade\n";
    cout << "[12] Run strategy simulation\n";
    cout << "[13] Compare all strategies\n";
    cout << "[14] Display portfolio summary\n";
    cout << "[15] Display trade history\n";
    cout << "[0]  Exit\n";

    cout << "\nEnter choice: ";
}

int main() {

    string studentName;
    string studentID;

    cout << "========================================\n";
    cout << "  ESE 224 StockSim - Student Login\n";
    cout << "========================================\n";

    cout << "Enter Student Name: ";
    getline(cin, studentName);

    cout << "Enter Student ID: ";
    getline(cin, studentID);

    StockManager<FinancialAsset> manager;

    Portfolio portfolio(100000);

    StockBST bst;

    Stock* amzn = nullptr;
    Stock* nvda = nullptr;
    ETF* spx = nullptr;

    // Prevents duplicate loading/allocation of the same assets.
    bool dataLoaded = false;

    int choice;

    do {

        printMenu(studentName, studentID);

        if (!(cin >> choice)) {

            cout << "Invalid choice.\n";

            cin.clear();
            cin.ignore(
                numeric_limits<streamsize>::max(),
                '\n'
            );

            continue;
        }

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );

        cout << endl;

        switch (choice) {

        case 1: {

            if (dataLoaded) {
                cout << "Data already loaded.\n";
                break;
            }

            cout << "Loading CSV data...\n";

            // Create asset objects once, then hydrate each with historical data.
            amzn = new Stock("AMZN", "Amazon Inc.");
            amzn->setSector("Technology");

            nvda = new Stock("NVDA", "NVIDIA Corp.");
            nvda->setSector("Technology");

            spx = new ETF("SPX", "S&P 500 Index", 0.03);
            spx->setSector("Index");

            amzn->loadFromCSV("data/AMZN.csv");

            nvda->loadFromCSV(
                "data/NVidia_stock_history.csv"
            );

            spx->loadFromCSV("data/SPX.csv");

            manager.addAsset(amzn);
            manager.addAsset(nvda);
            manager.addAsset(spx);

            dataLoaded = true;

            cout << "CSV data loaded successfully.\n";

            break;
        }

        case 2: {

            if (!dataLoaded) {
                cout << "Load data first.\n";
                break;
            }

            string ticker;

            cout << "Enter ticker: ";
            cin >> ticker;

            FinancialAsset* asset =
                manager.findByTicker(ticker);

            if (asset == nullptr) {
                cout << "Ticker not found.\n";
                break;
            }

            // Runtime type checks let us call the right history implementation.
            Stock* stock =
                dynamic_cast<Stock*>(asset);

            ETF* etf =
                dynamic_cast<ETF*>(asset);

            if (stock != nullptr) {

                PriceHistory* history =
                    stock->getHistory();

                for (PriceHistory::Iterator it =
                    history->begin();
                    it != history->end();
                    ++it) {

                    PriceNode& node = *it;

                    cout << node.date
                         << " Close: "
                         << node.close << endl;
                }
            }

            else if (etf != nullptr) {

                PriceHistory* history =
                    etf->getHistory();

                for (PriceHistory::Iterator it =
                    history->begin();
                    it != history->end();
                    ++it) {

                    PriceNode& node = *it;

                    cout << node.date
                         << " Close: "
                         << node.close << endl;
                }
            }

            break;
        }

        case 3: {

            string ticker;
            string startDate;
            string endDate;

            cout << "Enter ticker: ";
            cin >> ticker;

            cout << "Enter start date (YYYY-MM-DD): ";
            cin >> startDate;

            cout << "Enter end date (YYYY-MM-DD): ";
            cin >> endDate;

            FinancialAsset* asset =
                manager.findByTicker(ticker);

            if (asset == nullptr) {
                cout << "Ticker not found.\n";
                break;
            }

            Stock* stock =
                dynamic_cast<Stock*>(asset);

            ETF* etf =
                dynamic_cast<ETF*>(asset);

            if (stock != nullptr) {

                stock->getHistory()->printRange(
                    startDate,
                    endDate
                );
            }

            else if (etf != nullptr) {

                etf->getHistory()->printRange(
                    startDate,
                    endDate
                );
            }

            break;
        }

        case 4: {

            double low;
            double high;

            cout << "Enter low return: ";
            cin >> low;

            cout << "Enter high return: ";
            cin >> high;

            // Search precomputed yearly-return nodes already inserted into the BST.
            vector<BSTNode*> results;

            bst.rangeSearch(low, high, results);

            cout << "\nMatching Stocks:\n";

            for (size_t i = 0;
                 i < results.size();
                 i++) {

                cout << results[i]->ticker
                     << " | Return: "
                     << results[i]->key
                     << "% | Year: "
                     << results[i]->year
                     << endl;
            }

            break;
        }

        case 5: {

            string ticker;
            int year;

            cout << "Enter ticker: ";
            cin >> ticker;

            cout << "Enter year: ";
            cin >> year;

            FinancialAsset* asset =
                manager.findByTicker(ticker);

            if (asset == nullptr) {
                cout << "Ticker not found.\n";
                break;
            }

            // Insert yearly performance for fast range queries later (option 4).
            double annualReturn =
                asset->calculateAnnualReturn(year);

            bst.insert(
                ticker,
                annualReturn,
                year
            );

            cout << "Inserted into BST.\n";

            break;
        }

        case 6: {

            int traversal;

            cout << "[1] Inorder\n";
            cout << "[2] Preorder\n";
            cout << "[3] Postorder\n";

            cout << "Choice: ";
            cin >> traversal;

            // Traversal choice helps visualize BST ordering and shape.
            if (traversal == 1) {
                bst.inorder();
            }

            else if (traversal == 2) {
                bst.preorder();
            }

            else if (traversal == 3) {
                bst.postorder();
            }

            break;
        }

        case 7: {

            string ticker;
            int shares;
            double price;
            string date;

            cout << "Ticker: ";
            cin >> ticker;

            cout << "Shares: ";
            cin >> shares;

            cout << "Price: ";
            cin >> price;

            cout << "Date: ";
            cin >> date;

            portfolio.buyShares(
                ticker,
                shares,
                price,
                date
            );

            break;
        }

        case 8: {

            string ticker;
            int shares;
            double price;
            string date;

            cout << "Ticker: ";
            cin >> ticker;

            cout << "Shares: ";
            cin >> shares;

            cout << "Price: ";
            cin >> price;

            cout << "Date: ";
            cin >> date;

            portfolio.sellShares(
                ticker,
                shares,
                price,
                date
            );

            break;
        }

        case 9: {

            Order order;

            cout << "Ticker: ";
            cin >> order.ticker;

            cout << "Type (MARKET/LIMIT): ";
            cin >> order.type;

            cout << "Side (BUY/SELL): ";
            cin >> order.side;

            cout << "Shares: ";
            cin >> order.shares;

            // Queueing decouples order submission from market-time execution.
            if (order.type == "LIMIT") {

                cout << "Target Price: ";
                cin >> order.targetPrice;
            }

            else {
                order.targetPrice = 0;
            }

            cout << "Submitted Date: ";
            cin >> order.submittedDate;

            portfolio.queueOrder(order);

            cout << "Order queued.\n";

            break;
        }

        case 10: {

            double currentPrice;
            string currentDate;

            cout << "Current Market Price: ";
            cin >> currentPrice;

            cout << "Current Date: ";
            cin >> currentDate;

            // Applies execution rules against the oldest pending order.
            portfolio.executeNextOrder(
                currentPrice,
                currentDate
            );

            break;
        }

        case 11: {

            portfolio.undoLastTrade();

            break;
        }

        case 12: {

            if (!dataLoaded) {
                cout << "Load data first.\n";
                break;
            }

            int strategyChoice;

            double monthlyCapital;

            int startYear;
            int endYear;

            cout << "\nChoose Strategy:\n";

            cout << "[1] Fixed SIP\n";
            cout << "[2] Dynamic SIP\n";
            cout << "[3] Golden Cross\n";
            cout << "[4] Momentum\n";

            cout << "Choice: ";
            cin >> strategyChoice;

            cout << "Monthly Capital: ";
            cin >> monthlyCapital;

            cout << "Start Year: ";
            cin >> startYear;

            cout << "End Year: ";
            cin >> endYear;

            // Strategy is selected at runtime, then run over the same SPX history.
            TradingStrategy* strategy = nullptr;

            if (strategyChoice == 1) {

                strategy =
                    new FixedSIPStrategy();
            }

            else if (strategyChoice == 2) {

                strategy =
                    new DynamicSIPStrategy();
            }

            else if (strategyChoice == 3) {

                strategy =
                    new GoldenCrossStrategy();
            }

            else if (strategyChoice == 4) {

                double threshold;

                cout << "Momentum Threshold: ";
                cin >> threshold;

                strategy =
                    new MomentumStrategy(
                        threshold
                    );
            }

            if (strategy != nullptr) {

                SimResult result =
                    strategy->backtest(
                        spx->getHistory(),
                        monthlyCapital,
                        startYear,
                        endYear
                    );

                strategy->printResult(result);

                // Explicit delete avoids leaks for dynamically chosen strategies.
                delete strategy;
            }

            break;
        }

        case 13: {

            if (!dataLoaded) {
                cout << "Load data first.\n";
                break;
            }

            double monthlyCapital;
            int startYear;
            int endYear;

            cout << "Monthly Capital: ";
            cin >> monthlyCapital;

            cout << "Start Year: ";
            cin >> startYear;

            cout << "End Year: ";
            cin >> endYear;

            FixedSIPStrategy fixedSip;

            DynamicSIPStrategy dynamicSip;

            GoldenCrossStrategy golden;

            MomentumStrategy momentum(5.0);

            // Side-by-side benchmark of all strategies over identical parameters.
            SimResult r1 =
                fixedSip.backtest(
                    spx->getHistory(),
                    monthlyCapital,
                    startYear,
                    endYear
                );

            SimResult r2 =
                dynamicSip.backtest(
                    spx->getHistory(),
                    monthlyCapital,
                    startYear,
                    endYear
                );

            SimResult r3 =
                golden.backtest(
                    spx->getHistory(),
                    monthlyCapital,
                    startYear,
                    endYear
                );

            SimResult r4 =
                momentum.backtest(
                    spx->getHistory(),
                    monthlyCapital,
                    startYear,
                    endYear
                );

            cout << fixed << setprecision(2);

            cout << "\n===========================================================\n";

            cout << left
                 << setw(25) << "Strategy"
                 << setw(15) << "Final Value"
                 << setw(15) << "Return %"
                 << setw(15) << "CAGR %"
                 << endl;

            cout << "===========================================================\n";

            cout << setw(25) << r1.strategyName
                 << setw(15) << r1.finalValue
                 << setw(15) << r1.totalReturn
                 << setw(15) << r1.cagr
                 << endl;

            cout << setw(25) << r2.strategyName
                 << setw(15) << r2.finalValue
                 << setw(15) << r2.totalReturn
                 << setw(15) << r2.cagr
                 << endl;

            cout << setw(25) << r3.strategyName
                 << setw(15) << r3.finalValue
                 << setw(15) << r3.totalReturn
                 << setw(15) << r3.cagr
                 << endl;

            cout << setw(25) << r4.strategyName
                 << setw(15) << r4.finalValue
                 << setw(15) << r4.totalReturn
                 << setw(15) << r4.cagr
                 << endl;

            break;
        }

        case 14: {

            portfolio.printHoldings();

            break;
        }

        case 15: {

            portfolio.printTradeHistory();

            break;
        }

        case 0: {

            cout << "Exiting program...\n";

            break;
        }

        default:

            cout << "Invalid choice.\n";
        }

    } while (choice != 0);

    cout << "\nGoodbye, "
         << studentName
         << "!\n";

    return 0;
}