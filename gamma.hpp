//
//  gamma.hpp
//  pgsql
//
//  Created by FuYongrui on 5/1/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#ifndef gamma_hpp
#define gamma_hpp

#include <iostream>
#include <stdio.h>
#include <pqxx/pqxx>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <typeinfo>
#include <ql/time/all.hpp>
#include <string>
using namespace std;

typedef boost::shared_ptr<pqxx::connection> Conn_Ptr;

void make_conn();


#endif /* gamma_hpp */
