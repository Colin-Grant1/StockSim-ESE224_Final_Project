cmake_minimum_required(VERSION 3.10)
project(stocksim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(stocksim
    main.cpp
    src/CircularQueue.cpp
    src/CSVParser.cpp
    src/DynamicSIPStrategy.cpp
    src/DynamicSIPStrategyClassic.cpp
    src/DynamicSIPStrategyFurther.cpp
    src/ETF.cpp
    src/FinancialAsset.cpp
    src/FixedSIPStrategy.cpp
    src/GoldenCrossStrategy.cpp
    src/MomentumStrategy.cpp
    src/OrderQueue.cpp
    src/Portfolio.cpp
    src/PriceHistory.cpp
    src/Stock.cpp
    src/StockBST.cpp
    src/TradeStack.cpp
    src/TradingStrategy.cpp
)
