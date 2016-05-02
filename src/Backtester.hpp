//
//  Backtester.hpp
//  pgsql
//
//  Created by FuYongrui on 5/1/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#ifndef Backtester_hpp
#define Backtester_hpp

#include <stdio.h>

#include "DataWarehouse.hpp"
#include "Options.hpp"
#include "SignalGenerator.hpp"
//#include "PortfolioEngine.hpp"


namespace GammaScalping
{
    
    
    class Backtester
    {
    public:
        Backtester(OptionDataWarehouse_Sptr op_data_ptr, StockDataWarehouse_Sptr stk_data_ptr, Volatility entry, Volatility exit) :
        OptionData_ptr(op_data_ptr),
        StockData_ptr(stk_data_ptr),
        Signal_Generator(entry, exit)/*,
                                      Portfolio_Engine(PortfolioEngine(op_data_ptr))*/
        {
            StockPriceTimeSeries = StockData_ptr->make_query();
        }
        
        void start_backtest()
        {
            for(auto tup : StockPriceTimeSeries)
            {
                auto current_date = tup[0].as<Date>();
                auto stock_price = tup[1].as<PriceType>();
                auto expiry_date = get_expiry(current_date);
                auto strike = get_strike(stock_price);
                cout << current_date << ' ' << stock_price << ' ' << expiry_date << ' ' << strike << endl;
                
                auto CallOption = boost::make_shared<Option>(current_date,
                                                             expiry_date,
                                                             stock_price,
                                                             strike,
                                                             CallPut::CALL,
                                                             OptionData_ptr);
                cout << CallOption->get_impv() << " ";
                auto PutOption = boost::make_shared<Option>(current_date,
                                                            expiry_date,
                                                            stock_price,
                                                            strike,
                                                            CallPut::PUT,
                                                            OptionData_ptr);
                cout << PutOption->get_impv() << endl;
                Signal_Generator.update_option(CallOption, PutOption);
                
                //auto signal = Signal_Generator.generate_signal();
                
                /*Portfolio_Engine.update_portfolio(signal,
                 current_date,
                 expiry_date,
                 stock_price,
                 strike);*/
            }
        }
        
    private:
        pqxx::result StockPriceTimeSeries;
        
        OptionDataWarehouse_Sptr OptionData_ptr;
        StockDataWarehouse_Sptr StockData_ptr;
        SignalGenerator Signal_Generator;
        //PortfolioEngine Portfolio_Engine;
        
        Date get_expiry(const Date& curr_date)
        {
            auto curr_month = curr_date.substr(5,2);
            Date expiry = "2011-01-19";
            
            const vector<Date> expiry_calendar = {
                "2011-01-19",
                "2011-02-19",
                "2011-03-19",
                "2011-04-16",
                "2011-05-21",
                "2011-06-18",
                "2011-07-16",
                "2011-08-20",
                "2011-09-17",
                "2011-10-22",
                "2011-11-19",
                "2011-12-17",
                "2012-01-21"};
            
            
            
            if (stoi(curr_month) < 12)
                expiry = expiry_calendar.at(stoi(curr_month));
            else
                expiry = expiry_calendar.at(12);
            
            return expiry;
        }
        
        StrikeType get_strike(const PriceType& stock_price)
        {
            StrikeType strk = (int)(stock_price + 0.5);
            
            return strk;
        }
        
    };
}
#endif /* Backtester_hpp */
