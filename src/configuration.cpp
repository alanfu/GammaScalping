//
//  configuration.cpp
//  pgsql
//
//  Created by FuYongrui on 5/6/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#include "configuration.hpp"
#include <boost/lexical_cast.hpp>
#include <stdio.h>

using namespace std;

void AppConfig::setConfigParams(const StringToStringMap& configMap)
{
    MapIter e = configMap.end();
    MapIter it;
    
    it = configMap.find(STRAT_NAME);
    if (it != e)
        strategyName = static_cast<string>(it->second);
    else
        cerr << "Failed to config strategy name." << endl;
    
    it = configMap.find(MOVING_WINDOW);
    if (it != e)
        movingWindow = boost::lexical_cast<int>(it->second);
    else
        cerr << "Failed to config moving window." << endl;
    
    it = configMap.find(ENTRY_VOL);
    if (it != e)
        entryVol = boost::lexical_cast<double>(it->second);
    else
        cerr << "Failed to config entry vol." << endl;
    
    it = configMap.find(EXIT_VOL);
    if (it != e)
        exitVol = boost::lexical_cast<double>(it->second);
    else
        cerr << "Failed to config exit vol." << endl;
}

void AppConfig::createAppConfig(const string& fileName)
{
    auto a = readConfigFile(fileName);
    setConfigParams(a);
}

StringToStringMap AppConfig::readConfigFile(const string& fileName)
{
    TwoDStringVector cfgFileData = parse(fileName);
    
    vector<string> row;
    StringToStringMap ret;
    
    for (auto it = cfgFileData.begin(); it != cfgFileData.end(); it++)
    {
        row = *it;
        string key = row[0];
        string val = row[1];
        
        if (key[0] == '#' || key.empty())
            continue;
        
        ret.insert(pair<string, string>(key, val));
    }
    
    
    return ret;
}


TwoDStringVector AppConfig::parse(const string fileName)
{
    ifstream file(fileName.c_str());
    
    TwoDStringVector ret;
    string line;
    ret.clear();
    
    if (file.is_open())
        while (file.good())
        {
            getline(file, line);
            ret.push_back(parseLine(line));
        }
    
    return ret;
}

vector<string> AppConfig::parseLine(const string& line)
{
    Tokenizer token(line, sep);
    vector<string> tokens;
    tokens.clear();
    
    for (auto it = token.begin(); it != token.end(); it++)
    {
        tokens.push_back(*it);
    }
    return tokens;
}

