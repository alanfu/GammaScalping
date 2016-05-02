//
//  MathUtility.h
//  pgsql
//
//  Created by FuYongrui on 5/1/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#ifndef MathUtility_h
#define MathUtility_h

namespace GammaScalping
{
    inline double R(const double z)
    {
        const std::vector<const double>
        a = { 0.319381530 , -0.356563782 , 1.781477937 , -1.821255978 , 1.330274429 };
        
        return z*(z*(z*(z*( a.at(4)*z+a.at(3) ) + a.at(2) ) + a.at(1) ) + a.at(0));
    }
    
    inline double
    normal_cdf (const double x){
        if ( x > 0 ){
            double result = 0.0 ;
            result = 1.0 - 1.0/sqrt(2.0*M_PI)*exp(-x*x/2.0)*R(1.0/(1.0 + 0.2316419*x));
            return result;
        }
        else{
            return 1.0-normal_cdf(-x) ;
        }
    }
    
    inline double
    normal_pdf (const double x){
        return (1.0/sqrt(2.0*M_PI)*exp(-x*x/2.0));
    }
    
    template <typename T>
    double root_newton(T &func, const double x1 = 0.0, const double x2 = 2.0, const double acc = 0.00001) {
        // input a functor & range( x1 , x2 ) in which you believe there is a root & accuracy level
        // the functor provided must have a dff() method
        
        const int MAX_NUM_TRIAL = 50000 ;
        double rtn = 0.5 * ( x1 + x2 ) ;
        
        for ( size_t i = 0 ; i < MAX_NUM_TRIAL ; ++i ){
            
            double  f = func( rtn );
            double df = func.diff( rtn );
            double dx = f/df;
            
            rtn -= dx;
            if ( (x1-rtn) * (rtn-x2) < 0.0 )
                
                throw std::runtime_error
                (" Jumped out of brackets in root_newton method, check your range! ");
            
            if ( fabs(dx) < acc )
                return rtn;
            
        }
        
        throw std::runtime_error
        (" Maximum number of iterations exceeded in rtnewt, may caused by accuracy level ");
        
    }
}

#endif /* MathUtility_h */
