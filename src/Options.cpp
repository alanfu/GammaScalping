//
//  Options.cpp
//  pgsql
//
//  Created by FuYongrui on 5/1/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#include "Options.hpp"

namespace GammaScalping
{
    
    PriceType BS_CALL_PRICE(const PriceType stock_price,
                            const PriceType strike,
                            const RateType rate,
                            const Volatility vol,
                            const YearFraction time_to_maturity)
    {
        double d1 = ( log(stock_price/strike) + (rate + 0.5*vol*vol)*time_to_maturity ) / ( vol*sqrt(time_to_maturity) ) ;
        double d2 = ( log(stock_price/strike) + (rate - 0.5*vol*vol)*time_to_maturity ) / ( vol*sqrt(time_to_maturity) ) ;
        return ( stock_price*normal_cdf(d1) - strike*exp(-rate*time_to_maturity)*normal_cdf(d2) );
    }
    
    PriceType BS_PUT_PRICE(const PriceType stock_price,
                           const PriceType strike,
                           const RateType rate,
                           const Volatility vol,
                           const YearFraction time_to_maturity)
    {
        double d1 = ( log(stock_price/strike) + (rate + 0.5*vol*vol)*time_to_maturity ) / ( vol*sqrt(time_to_maturity) ) ;
        double d2 = ( log(stock_price/strike) + (rate - 0.5*vol*vol)*time_to_maturity ) / ( vol*sqrt(time_to_maturity) ) ;
        return ( -stock_price*normal_cdf(-d1) + strike*exp(-rate*time_to_maturity)*normal_cdf(-d2) );
    }
    
    Volatility BS_VEGA(const PriceType stock_price,
                       const PriceType strike,
                       const RateType rate,
                       const Volatility vol,
                       const YearFraction time_to_maturity)
    {
        double d1 = ( log(stock_price/strike) + (rate + 0.5*vol*vol)*time_to_maturity ) / ( vol*sqrt(time_to_maturity) ) ;
        return stock_price * sqrt(time_to_maturity) * normal_pdf(d1);
    }
    
    void Find_Root_Helper::set_params(const PriceType _stock_price,
                                      const StrikeType _strike,
                                      const YearFraction _time_to_maturity,
                                      const CallPut _call_put,
                                      const PriceType _option_price)
    {
        stock_price = _stock_price;
        strike = _strike;
        time_to_maturity = _time_to_maturity;
        call_put = _call_put;
        option_price = _option_price;
    }
    
    Volatility Find_Root_Helper::operator()(Volatility vol)
    {
        if (call_put == CallPut::CALL)
            return BS_CALL_PRICE(stock_price,
                                 strike,
                                 rate,
                                 vol,
                                 time_to_maturity) - option_price;
        else
            return BS_PUT_PRICE(stock_price,
                                strike,
                                rate,
                                vol,
                                time_to_maturity) - option_price;
    }
    
    Volatility Find_Root_Helper::diff(Volatility vol)
    {
        return BS_VEGA(stock_price,
                       strike,
                       rate,
                       vol,
                       time_to_maturity);
    }
    
    
}
