//
//  DataWarehouse.hpp
//  pgsql
//
//  Created by FuYongrui on 5/1/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#ifndef DataWarehouse_hpp
#define DataWarehouse_hpp

#include <stdio.h>

#include <iostream>
#include <pqxx/pqxx>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <ql/time/all.hpp>

#define DEBUG = false

using namespace std;

namespace GammaScalping
{
    
    typedef double PriceType;
    typedef int StrikeType;
    typedef double ShareType;
    typedef double RateType;
    typedef double PNLType;
    typedef string Date;
    
    enum CallPut {CALL, PUT};
    
    typedef boost::shared_ptr<pqxx::connection> Conn_Ptr;
    
    class OptionDataWarehouse;
    typedef boost::shared_ptr<OptionDataWarehouse> OptionDataWarehouse_Sptr;
    
    class StockDataWarehouse;
    typedef boost::shared_ptr<StockDataWarehouse> StockDataWarehouse_Sptr;
    
    class DataWarehouse
    {
    public:
        
        DataWarehouse()
        {
            try{
                conn = boost::make_shared<pqxx::connection>("dbname=mydb user=alanfu hostaddr=127.0.0.1 port=5432");
                
                if (conn->is_open()) {
                    cout << "Opened database successfully: " << conn->dbname() << endl;
                } else {
                    throw runtime_error("Cannot connect to database!");
                }
            }catch (runtime_error &e){
                cerr << e.what() << std::endl;
            }
        }
        
    protected:
        Conn_Ptr conn;
        Date current_date;
    private:
        
    };// class DataWarehouse
    
    class StockDataWarehouse : public DataWarehouse
    {
    public:
        StockDataWarehouse() : DataWarehouse()
        {
            cout << "Connect to stock database successfully." << endl;
        }
        
        void update_date(const Date& curr_date)
        {
            current_date = curr_date;
        }
        
        pqxx::result make_query()
        {
            pqxx::work W(*conn);
            
            string temp = "SELECT * FROM spystocks;";
            const char* sql=temp.c_str();
            
            pqxx::result R = W.exec(sql);
            //W.commit();
            return R;
        }
        
    protected:
        
    private:
        
    };//class StockDataWarehouse
    
    class OptionDataWarehouse : public DataWarehouse
    {
    public:
        OptionDataWarehouse() : DataWarehouse()
        {
            cout << "Connect to option database successfully." << endl;
        }
        /*
        inline void set_params(const Date& curr_date,
                               const Date& ex_date,
                               const StrikeType& stk,
                               const CallPut& c_p)
        {
            current_date = curr_date;
            expiry_date = ex_date;
            strike = stk;
            call_put = c_p;
        }
        */
        PriceType make_query(const Date& curr_date,
                             const Date& ex_date,
                             const StrikeType& stk,
                             const CallPut& c_p)
        {
            pqxx::work W(*conn);
            string optiontable = "spyoptions";
            string call_put = (c_p == CallPut::CALL) ? "C" : "P";
            string temp = "select price from spyoptions where cur_date = date \'" + curr_date + "\' and ex_date = date \'" + ex_date + "\' and flag = \'" + call_put + "\' and k = " + to_string(stk) + ";";
            const char* sql = temp.c_str();
        
            pqxx::result R = W.exec(sql);
            PriceType ret = stof(R.begin()[0].as<string>());
            return ret;
        }
        
    private:
        /*
        Date expiry_date;
        StrikeType strike;
        CallPut call_put;
        */
    };// class OptionDataWarehouse
}

#endif /* DataWarehouse_hpp */
