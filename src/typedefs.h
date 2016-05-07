//
//  typedefs.h
//  pgsql
//
//  Created by FuYongrui on 5/6/16.
//  Copyright © 2016 FuYongrui. All rights reserved.
//

#ifndef typedefs_h
#define typedefs_h

#include <string>
#include <vector>
#include <map>
#include <boost/tokenizer.hpp>
using namespace std;

static const boost::char_separator<char> sep("#;,=");
typedef vector<vector<string>> TwoDStringVector;
typedef multimap<string, string> StringToStringMap;
typedef boost::tokenizer<boost::char_separator<char>> Tokenizer;
typedef StringToStringMap::const_iterator MapIter;


#endif /* typedefs_h */
