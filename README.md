# C++ Trading System & Historical Backtesting Engine

A modular C++ framework for evaluating systematic investment strategies on historical market data. The project explores strategy implementation, portfolio simulation, and the trade-off between model complexity and measurable performance.

## Overview

The system provides a reusable backtesting framework for comparing multiple investment strategies under a common portfolio and execution model.

Implemented strategies include:

- **Fixed SIP** — periodic investment of a fixed amount.
- **Dynamic SIP** — adjusts capital deployment using market trends, drawdown, momentum, volatility, and cash-reserve constraints.
- **Momentum** — allocates based on momentum signals.
- **Golden Cross** — uses moving-average crossover signals.

Strategies were evaluated across approximately 20 years of historical data for:

- S&P 500 Index (SPX)
- NVIDIA (NVDA)
- Amazon (AMZN)

## Features

- Object-oriented C++ architecture
- Polymorphic strategy interfaces
- Custom containers for portfolio and position management
- Historical market-data processing
- Simulated trade execution and portfolio accounting
- Fixed-budget capital deployment
- Strategy parameter sensitivity analysis
- CMake-based build system

## Dynamic SIP Strategy

The Dynamic SIP strategy extends fixed periodic investing by adapting capital allocation to market conditions.

It incorporates:

- Moving-average trend signals
- Drawdown-based position sizing
- Momentum confirmation
- Volatility adjustment
- Cash-reserve management
- Fixed-budget constraints

## Results

Parameter sensitivity testing showed **negligible performance improvement over Fixed SIP despite substantially greater model complexity** across the tested historical datasets.

This result demonstrated the value of using the framework not only to search for higher-performing strategies, but also to evaluate whether additional model complexity was justified by measurable gains.

## Architecture

Strategies are separated from the underlying backtesting infrastructure through a common interface, allowing new strategies to be added without modifying portfolio, market-data, or execution components.


## Technologies

- **C++**
- **STL**
- **CMake**

## Building/ Compilation

Option A — CMake
mkdir build && cd build && cmake .. && make
./stocksim

Option B — Direct g++
g++ -std=c++11 -Iinclude src/*.cpp main.cpp -o stocksim
./stocksim
./stocksim

Run the resulting executable according to the project configuration.

## Disclaimer

This project is an educational backtesting system and is not intended to provide financial advice or predict future market performance. Historical results do not guarantee future returns.
