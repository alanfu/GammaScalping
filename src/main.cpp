//
//  main.cpp
//  pgsql
//
//  Created by FuYongrui on 12/10/15.
//  Copyright © 2015 FuYongrui. All rights reserved.
//

#include <iostream>

#include "gamma.hpp"
#include "DataWarehouse.hpp"
#include "Options.hpp"
#include "SignalGenerator.hpp"
#include "Backtester.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    auto StockData_ptr = boost::make_shared<GammaScalping::StockDataWarehouse>();
    auto OptionData_ptr = boost::make_shared<GammaScalping::OptionDataWarehouse>();
    
    GammaScalping::Volatility entry_signal = 0.2;
    GammaScalping::Volatility exit_signal = 0.3;

    GammaScalping::Backtester my_backtest(OptionData_ptr,
                                          StockData_ptr,
                                          entry_signal,
                                          exit_signal);
    
    my_backtest.start_backtest();
    
    //make_conn();
    
    
}


