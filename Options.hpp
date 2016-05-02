//
//  Options.hpp
//  pgsql
//
//  Created by FuYongrui on 5/1/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#ifndef Options_hpp
#define Options_hpp

#include <stdio.h>
#include "DataWarehouse.hpp"
#include "MathUtility.h"
#include <ql/utilities/dataparsers.hpp>

static const QuantLib::Calendar   calendar = QuantLib::UnitedStates();
static const QuantLib::DayCounter day_counter  = QuantLib::Business252(calendar);

namespace GammaScalping
{
    typedef double Volatility;
    typedef double YearFraction;
    
    PriceType BS_CALL_PRICE(const PriceType stock_price,
                            const PriceType strike,
                            const RateType rate,
                            const Volatility vol,
                            const YearFraction time_to_maturity);
    
    PriceType BS_PUT_PRICE(const PriceType stock_price,
                           const PriceType strike,
                           const RateType rate,
                           const Volatility vol,
                           const YearFraction time_to_maturity);
    
    Volatility BS_VEGA(const PriceType stock_price,
                       const PriceType strike,
                       const RateType rate,
                       const Volatility vol,
                       const YearFraction time_to_maturity);
    
    class Option;
    typedef boost::shared_ptr<Option> Option_Sptr;
    
    struct Find_Root_Helper
    {
        PriceType stock_price;
        StrikeType strike;
        YearFraction time_to_maturity;
        CallPut call_put;
        PriceType option_price;
        RateType rate = 0.05;
        void set_params(const PriceType _stock_price,
                        const StrikeType _strike,
                        const YearFraction _time_to_maturity,
                        const CallPut _call_put,
                        const PriceType _option_price);
        Volatility operator()(Volatility vol);
        Volatility diff(Volatility vol);
    };
    
    class Option
    {
    public:
        Option(const Date& curr_date,
               const Date& ex_date,
               const PriceType& _stock_price,
               const StrikeType& stk,
               const CallPut& c_p,
               OptionDataWarehouse_Sptr op_data_ptr) :
        current_date(curr_date),
        expiry_date(ex_date),
        stock_price(_stock_price),
        strike(stk),
        call_put(c_p),
        option_data_ptr(op_data_ptr)
        {
            latest_mkt_price(curr_date, _stock_price);
        }
        
        // this function not only update the options price, but also update all other related metrics
        void latest_mkt_price(const Date& curr_date, const PriceType& stock_price)
        {
            update_date(curr_date);
            /*
            option_data_ptr->set_params(current_date,
                                        expiry_date,
                                        strike,
                                        call_put);
            */
            option_price = option_data_ptr->make_query(current_date,
                                                       expiry_date,
                                                       strike,
                                                       call_put);
            
            auto d1 = QuantLib::DateParser::parseFormatted(current_date, "%Y-%m-%d");
            auto d2 = QuantLib::DateParser::parseFormatted(expiry_date, "%Y-%m-%d");
            time_to_maturity = day_counter.yearFraction(d1, d2);
            
            frh.set_params(stock_price,
                           strike,
                           time_to_maturity,
                           call_put,
                           option_price);
            
            calc_impv();
        }
        
        void update_delta()
        {
            double d1 = ( log(option_price/strike) + (rate + 0.5*vol*vol)*time_to_maturity ) / ( vol*sqrt(time_to_maturity) ) ;
            if (call_put == CallPut::CALL)
                delta = normal_cdf(d1);
            else
                delta = normal_cdf(d1) - 1;
        }
        
        void calc_impv()
        {
            vol = root_newton(frh);
        }
        
        double get_delta()
        {
            return delta;
        }
        
        Volatility get_impv()
        {
            return vol;
        }
        
        PNLType option_pnl(const Date& curr_date)
        {
            PNLType pnl = 0;
            
            return pnl;
        }
        
        
    protected:
        Date current_date;
        Date expiry_date;
        PriceType stock_price;
        StrikeType strike;
        CallPut call_put;
        OptionDataWarehouse_Sptr option_data_ptr;
        YearFraction time_to_maturity;
        PriceType option_price;
        Volatility vol;
        RateType rate = 0.05;
        double delta;
        Find_Root_Helper frh;
        
    private:
        inline void update_date(const Date& curr_date)
        {
            current_date = curr_date;
        }
    };// class Option
    
}
#endif /* Options_hpp */
