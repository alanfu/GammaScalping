//
//  SignalGenerator.hpp
//  pgsql
//
//  Created by FuYongrui on 5/1/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#ifndef SignalGenerator_hpp
#define SignalGenerator_hpp

#include <stdio.h>

#include "DataWarehouse.hpp"
#include "Options.hpp"
#include <list>
//#include <numeric>
#include <boost/range/numeric.hpp>

namespace GammaScalping
{
    enum Signal {ENTER,KEEP,EXIT};//Signals
    
    class SignalGenerator
    {
    public:
        
        SignalGenerator(Volatility entry = 0.2, Volatility exit = 0.3)
        {
            entry_vol = entry;
            exit_vol = exit;
        }
        
        Signal generate_signal()
        {
            Signal signal = Signal::KEEP;
            
            Volatility vol = (CallOption->get_impv() + PutOption->get_impv()) * 0.5;
            
            if(vol_queue.size() == 20)
            {
                Volatility moving_average = boost::accumulate(vol_queue, 0.0) / window_size;
                
                vol_queue.push_front(moving_average);
                vol_queue.pop_back();
                
            }
            else if(vol_queue.size() > 20)
                throw runtime_error("too much element in volatility queue!");
            else
                vol_queue.push_front(vol);
            
            
            
            
            return signal;
        }
        
        void update_option(Option_Sptr option_call, Option_Sptr option_put)
        {
            CallOption = option_call;
            PutOption = option_put;
        }
        
    private:
        list<Volatility> vol_queue;
        int window_size = 20;
        Volatility entry_vol;
        Volatility exit_vol;
        Option_Sptr CallOption;
        Option_Sptr PutOption;
    };
}
#endif /* SignalGenerator_hpp */
