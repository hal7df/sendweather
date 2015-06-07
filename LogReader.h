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
    LogReader();

    struct tm getLastWuUpdate(string fname);
    weatherData parseLogLine(string line); // log produced by run2300d
    weatherData parseHistlogLine(string line); // log produced by histlog2300
    struct tm getLogTime(string line);
    
private:
    vector<string> splitLogLine (string line);
    string readLastLine (string fname);
};

#endif //LOGREADER_H