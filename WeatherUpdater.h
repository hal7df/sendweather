#ifndef WEATHERUPDATER_H
#define WEATHERUPDATER_H

#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include <ctime>
#include <cstdio>
#include <cstdlib>

#include <curl/curl.h>

#include "LogReader.h"
#include "WuUpload.h"

using namespace std;

class WeatherUpdater
{
public:
    WeatherUpdater(string fname, string userID, string passwd);
    ~WeatherUpdater();

    bool fetchLastUpdate();
    bool uploadUnattended();
    
    CURLcode uploadWeatherData(weatherData point);
    
    size_t writeData(void *data, size_t size, size_t nmemb, FILE *stream);
    
private:
    bool downloadHistoryFile();
    void findLastUpdate();
    void readUnattendedData();
    
    string getWuPostString (weatherData point);
    string formatWuTime (struct tm ptTm);
    
    LogReader* m_logreader;
    
    struct tm m_lastUpdate;
    vector<weatherData> m_unattendedData;
    string m_userID, m_passwd;
    string m_fname;
    
    bool f_uploadWeek;
};

#endif //WEATHERUPDATER_H