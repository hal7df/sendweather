#ifndef WEATHERDAEMON_H
#define WEATHERDAEMON_H

#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#include <ctime>

#include <curl/curl.h>

#include "inotify/inotify-cxx.h"
#include "LogReader.h"
#include "WuUpload.h"

#define SENDWEATHERD_NOERROR 0
#define SENDWEATHERD_POSTERROR 1
#define SENDWEATHERD_WATCHERROR 2
#define SENDWEATHERD_STDERROR 3
#define SENDWEATHERD_UNKERROR 4
#define SENDWEATHERD_FATALERROR 5

class WeatherDaemon
{
public:
    WeatherDaemon(string userID, string passwd, string fname, string config);
    ~WeatherDaemon();
    
    void runDaemon();
    
    int getErrorCode() { return m_daemonError; }
    string getErrorString();
    
    void stopDaemon() { f_endDaemon = true; }
    
private:
    CURLcode uploadWeatherData(weatherData point);
    bool uploadCachedData();
    
    string getLogFilename();
    string getLogPath();
    
    LogReader* m_logreader;
    
    vector<weatherData> m_awaitingUpload;
    
    string m_fname;
    string m_userID;
    string m_passwd;
    string m_config;
    
    int m_daemonError;
    bool f_endDaemon;
}

#endif //WEATHERDAEMON_H