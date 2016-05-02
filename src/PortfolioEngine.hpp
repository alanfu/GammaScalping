//
//  PortfolioEngine.hpp
//  pgsql
//
//  Created by FuYongrui on 5/1/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#ifndef PortfolioEngine_hpp
#define PortfolioEngine_hpp

#include <stdio.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <tuple>
#include <pqxx/pqxx>
#include "DataWarehouse.hpp"
#include "Options.hpp"
#include "SignalGenerator.hpp"
using namespace std;
using boost::shared_ptr;


namespace GammaScalping
{
    using namespace pqxx;
    
    enum LongShort {LONG, SHORT};
    enum DeltaHedgeSignal {BUY_TO_HEDGE, SELL_TO_HEDGE, NONE};
    
    struct stock_transaction
    {
        Date current_date;
        ShareType num_shares;
        PriceType price;
        LongShort long_short;
    };
    
    typedef typename std::vector<stock_transaction> stock_portfolio;
    typedef typename std::tuple<Option_Sptr,Option_Sptr> option_portfolio;
    typedef typename std::tuple<DeltaHedgeSignal,ShareType> dh_transaction;
    
    class PortfolioEngine
    {
    private:
        
        struct portfolio_total
        {
            portfolio_total():delta(0) {}
            stock_portfolio Stock_Portfolio;
            option_portfolio Option_Portfolio;
            double delta;
        };
        
        portfolio_total Portfolio_All;
        vector<PNLType> PNL;
        OptionDataWarehouse_Sptr OptionData_ptr;
        bool is_gamma_scalp = false;
        
    public:
        
        PortfolioEngine(OptionDataWarehouse_Sptr op_data_ptr) : OptionData_ptr(op_data_ptr) {}
        
        void update_portfolio(const Signal& sig,
                              const Date& curr_date,
                              const Date& expiry_date,
                              const PriceType& st_price,
                              const StrikeType& strk);
        
    private:
        
        dh_transaction init_option_portfolio(const Date& curr_date,
                                             const Date& ex_date,
                                             const PriceType& st_price,
                                             const StrikeType& strike);
        
        void init_stock_portfolio(const dh_transaction& dh_t,
                                  const Date& curr_date,
                                  const PriceType& stock_price);
        
        dh_transaction update_option_portfolio(const Date& curr_date,
                                               const PriceType& st_price);
        
        PNLType update_stock_portfolio(const dh_transaction& dht,
                                       const Date& curr_date,
                                       const PriceType& stock_price);
        
        PNLType close_option_portfolio(const Date& curr_date);
        
        PNLType close_stock_portfolio(const PriceType& stock_price);
        
        
        
        /************ Utility functions ************/
        void update_price_n_delta(const Date& curr_date, const PriceType& stock_price)
        {
            get<0>(Portfolio_All.Option_Portfolio)->latest_mkt_price(curr_date,
                                                                     stock_price);
            get<1>(Portfolio_All.Option_Portfolio)->latest_mkt_price(curr_date,
                                                                     stock_price);
            get<0>(Portfolio_All.Option_Portfolio)->update_delta();
            get<1>(Portfolio_All.Option_Portfolio)->update_delta();
        }
        
        dh_transaction delta_hedge_signal()
        {
            auto temp_call = get<0>(Portfolio_All.Option_Portfolio);
            auto temp_put = get<1>(Portfolio_All.Option_Portfolio);
            auto delta_total = temp_call->get_delta() + temp_put->get_delta();
            
            DeltaHedgeSignal dhs = DeltaHedgeSignal::NONE;
            
            if (delta_total > 0.15)
                dhs = DeltaHedgeSignal::SELL_TO_HEDGE;
            else if(delta_total < -0.15)
                dhs = DeltaHedgeSignal::BUY_TO_HEDGE;
            
            ShareType shares = abs(delta_total);// shares *= 100
            
            dh_transaction dh_t = std::make_tuple(dhs,shares);
            
            return dh_t;
        }
        
        inline void add_stock_trade(const dh_transaction& dh_t,
                                    const Date& curr_date,
                                    const PriceType& stock_price)
        {
            // add trade to empty vector || existing trade and new trade has same direction
            stock_transaction new_stock;
            new_stock.current_date = curr_date;
            new_stock.num_shares = get<1>(dh_t);
            new_stock.price = stock_price;
            new_stock.long_short = (get<0>(dh_t) == DeltaHedgeSignal::BUY_TO_HEDGE) ? LongShort::LONG : LongShort::SHORT;
            Portfolio_All.Stock_Portfolio.push_back(new_stock);
        }
        
    };
}

#endif /* PortfolioEngine_hpp */
