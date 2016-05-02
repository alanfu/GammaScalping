//
//  PortfolioEngine.cpp
//  pgsql
//
//  Created by FuYongrui on 5/1/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#include "PortfolioEngine.hpp"

namespace GammaScalping
{
    void PortfolioEngine::update_portfolio(const Signal& sig,
                                           const Date& curr_date,
                                           const Date& expiry_date,
                                           const PriceType& st_price,
                                           const StrikeType& strk)
    {
        
        PriceType stock_price = st_price;
        StrikeType atm = strk;
        Date ex_date = expiry_date;
        PNLType pnl = 0;
        
        switch (sig) {
                
            case Signal::ENTER:
            {
                if (!is_gamma_scalp){
                    
                    // init option position and generate delta hedge signal
                    dh_transaction init_trade = init_option_portfolio(curr_date,
                                                                      ex_date,
                                                                      stock_price,
                                                                      atm);
                    
                    if(get<0>(init_trade) != DeltaHedgeSignal::NONE)
                        init_stock_portfolio(init_trade, curr_date, stock_price);
                    // update stock position
                    is_gamma_scalp = true;
                    break;
                }
            }
                
            case Signal::KEEP:
            {
                dh_transaction dh_trade = update_option_portfolio(curr_date,
                                                                  stock_price);// generate delta hedge signal
                if(get<0>(dh_trade) != DeltaHedgeSignal::NONE)
                    pnl += update_stock_portfolio(dh_trade, curr_date, stock_price);// update stock position and calculate pnl
                break;
            }
                
            case Signal::EXIT:
            {
                pnl += close_option_portfolio(curr_date);// calculate option pnl
                pnl += close_stock_portfolio(stock_price);// calculate stock pnl
                break;
            }
        }
        
        PNL.push_back(pnl);
    }
    
    dh_transaction PortfolioEngine::init_option_portfolio(const Date& curr_date,
                                                          const Date& ex_date,
                                                          const PriceType& stock_price,
                                                          const StrikeType& strike)
    {
        auto new_call = boost::make_shared<Option>(curr_date,
                                                   ex_date,
                                                   stock_price,
                                                   strike,
                                                   CallPut::CALL,
                                                   OptionData_ptr);
        
        auto new_put = boost::make_shared<Option>(curr_date,
                                                  ex_date,
                                                  stock_price,
                                                  strike,
                                                  CallPut::PUT,
                                                  OptionData_ptr);
        
        Portfolio_All.Option_Portfolio = std::make_tuple(new_call,new_put);
        
        update_price_n_delta(curr_date, stock_price);
        
        return delta_hedge_signal();
    }
    
    void PortfolioEngine::init_stock_portfolio(const dh_transaction& dh_t,
                                               const Date& curr_date,
                                               const PriceType& stock_price)
    {
        add_stock_trade(dh_t, curr_date, stock_price);
    }
    
    dh_transaction PortfolioEngine::update_option_portfolio(const Date& curr_date,
                                                            const PriceType& stock_price)
    {
        update_price_n_delta(curr_date, stock_price);
        
        return delta_hedge_signal();
        
    }
    
    PNLType PortfolioEngine::update_stock_portfolio(const dh_transaction& dht,
                                                    const Date& curr_date,
                                                    const PriceType& stock_price)
    {
        PNLType realized_pnl = 0;
        //PNLType unrealized_pnl;
        auto dh_signal = get<0>(dht);
        auto stock_port_tmp = Portfolio_All.Stock_Portfolio;
        
        if(stock_port_tmp.empty()) // if there is no transaction record
            add_stock_trade(dht, curr_date, stock_price);
        else // if there are transaction records
        {
            auto curr_direction = stock_port_tmp[0].long_short;
            
            // if existing direction is long and we need to buy to hedge
            if(curr_direction == LongShort::LONG && dh_signal == DeltaHedgeSignal::BUY_TO_HEDGE)
                add_stock_trade(dht, curr_date, stock_price);
            
            // if existing direction is short and we need to sell to hedge
            else if(curr_direction == LongShort::SHORT && dh_signal == DeltaHedgeSignal::SELL_TO_HEDGE)
                add_stock_trade(dht, curr_date, stock_price);
            
            // if direction is different
            else
            {
                auto new_shares = get<1>(dht);
                
                // we close existing positions from most recent
                for(auto it = stock_port_tmp.rbegin(); it != stock_port_tmp.rend(); it++)
                {
                    if(it->num_shares > new_shares)
                    {
                        PNLType pnl_abs = new_shares * (stock_price - it->price);
                        
                        if(dh_signal == DeltaHedgeSignal::BUY_TO_HEDGE)
                            realized_pnl -= pnl_abs;
                        else
                            realized_pnl += pnl_abs;
                        
                        it->num_shares -= new_shares;
                        
                        break;
                    }
                    else
                    {
                        auto pnl_abs = it->num_shares * (stock_price - it->price);
                        
                        if(dh_signal == DeltaHedgeSignal::BUY_TO_HEDGE)
                            realized_pnl -= pnl_abs;
                        else
                            realized_pnl += pnl_abs;
                        
                        new_shares -= it->num_shares;
                        
                        stock_port_tmp.pop_back(); // *it has been close by new trade
                        
                    }
                }
                
                // all the existing portfolio have been close
                if(new_shares > 0)
                {
                    auto dh_new = std::make_tuple(dh_signal, new_shares);
                    add_stock_trade(dh_new, curr_date, stock_price);
                }
            }
        }
        
        return realized_pnl;
    }
    
    PNLType PortfolioEngine::close_option_portfolio(const Date& curr_date)
    {
        PNLType realized_pnl = 0;
        
        realized_pnl += get<0>(Portfolio_All.Option_Portfolio)->option_pnl(curr_date);
        realized_pnl += get<1>(Portfolio_All.Option_Portfolio)->option_pnl(curr_date);
        
        return realized_pnl;
    }
    
    PNLType PortfolioEngine::close_stock_portfolio(const PriceType& stock_price)
    {
        PNLType realized_pnl = 0;
        
        auto stock_port_tmp = Portfolio_All.Stock_Portfolio;
        
        if(!stock_port_tmp.empty())
        {
            PNLType pnl_abs = 0;
            
            for(auto it = stock_port_tmp.rbegin(); it != stock_port_tmp.rend(); it++)
                pnl_abs += stock_price - it->price;
            
            auto curr_direction = stock_port_tmp[0].long_short;
            
            if(curr_direction == LongShort::SHORT)
                realized_pnl -= pnl_abs;
            else
                realized_pnl += pnl_abs;
        }
        
        return realized_pnl;
    }
    
}