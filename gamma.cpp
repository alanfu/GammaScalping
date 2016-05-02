//
//  gamma.cpp
//  pgsql
//
//  Created by FuYongrui on 5/1/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#include "gamma.hpp"
using namespace std;

string stocktable="SPYSTOCKS";
string optiontable="SPYOPTIONS";
string recordtable="RECORDS";

void make_conn()
{
    Conn_Ptr conn;
    try{
        conn = boost::make_shared<pqxx::connection>("dbname=mydb user=alanfu hostaddr=127.0.0.1 port=5432");
        
        if (conn->is_open()) {
            cout << "Opened database successfully: " << conn->dbname() << endl;
        } else {
            throw runtime_error("Cannot connect to database!");
        }
        
        pqxx::work W(*conn);
        
        string temp = "SELECT * FROM "+stocktable+";";
        const char* sql=temp.c_str();
        
        pqxx::result R = W.exec(sql);
        
        pqxx::result::const_iterator iter=R.begin();
        
        for(auto tup : R)
        {
            cout << tup[1].as<int>() << endl;
            cout << stoi(tup[0].as<string>().substr(5,2)) + 1 << endl;
        }
        
        string cur_date = "2011-01-03";
        string ex_date = "2011-01-07";
        string callput = "C";
        temp = "select price from " + optiontable + " where cur_date = date \'" + cur_date + "\' and ex_date = date \'" + ex_date + "\' and flag = \'" + callput + "\' and k = 113;";
        sql=temp.c_str();
        
        R = W.exec(sql);
        cout << stof(R.begin()[0].as<string>()) + 1 << endl;
        W.commit();
        conn->disconnect ();
        
    }catch (runtime_error &e){
        cerr << e.what() << std::endl;
    }
}