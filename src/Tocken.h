//
// Created by nadav on 08/01/16.
//

#ifndef SPL_CLIENT_TOCKEN_H
#define SPL_CLIENT_TOCKEN_H

#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
using namespace std;

class Tocken {
public:
    string command;
    vector<string> params;
    Tocken(string msg){
        params =  boost::split(params,msg,boost::is_any_of(" "));
        command=params.front();
        params.erase(params.begin());
    }

};


#endif //SPL_CLIENT_TOCKEN_H
