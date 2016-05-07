//
//  configuration.hpp
//  pgsql
//
//  Created by FuYongrui on 5/6/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#ifndef configuration_hpp
#define configuration_hpp

#include <stdio.h>

#include "typedefs.h"
#include <boost/tokenizer.hpp>
#include <iostream>
#include <fstream>
#include "constants.h"

using namespace std;



struct AppConfig
{
private:
    //=========== configuration parameters ==========//
    
    string strategyName;
    int movingWindow;
    double entryVol;
    double exitVol;
    
public:
    void setConfigParams(const StringToStringMap& configMap);
    
    void createAppConfig(const string& fileName);
    
    StringToStringMap readConfigFile(const string& fileName);
    
    const string& getStratName() const
    {
        return strategyName;
    }
    
private:
    
    //=========== utility functions ==========//
    
    TwoDStringVector parse(const string fileName);
    
    inline vector<string> parseLine(const string& line);
};

#endif /* configuration_hpp */
