#ifndef LOGREADER_H
#define LOGREADER_H

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>

#include "WeatherData.h"
using namespace std;

class LogReader 
{
public:
    LogReader() {}
    virtual ~LogReader {}

    struct tm getLastWuUpdate(string fname);
    struct tm getLogTime(string line);
    
    weatherData parseLogLine(string line); // log produced by run2300d
    weatherData parseHistlogLine(string line); // log produced by histlog2300
    
    string readLastLine (string fname);
    
private:
    vector<string> splitLogLine (string line);
};

#endif //LOGREADER_H