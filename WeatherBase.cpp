#include "WeatherBase.h"

/** CTOR **/
WeatherBase::WeatherBase (string userID, string passwd)
{
    m_userID = userID;
    m_passwd = passwd;
    
    curl_global_init(CURL_GLOBAL_ALL);
}

/** DTOR **/
WeatherBase::~WeatherBase()
{
    curl_global_cleanup();
}


/**** PUBLIC FUNCTIONS ****/

CURLcode WeatherBase::uploadWeatherData (weatherData point)
{
    CURL* curl;
    CURLcode curlerr;
    
    string postData;
   
    postData = getWuPostString(point);
    
    curlerr = CURLE_FAILED_INIT;
    curl = curl_easy_init();
    
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, WU_UPLOAD_URL);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
        
        curlerr = curl_easy_perform(curl);
        
        curl_easy_cleanup(curl);
    }
    
    return curlerr;
}

bool WeatherBase::bulkUpload (vector<weatherData>* points)
{
    CURL* curl;
    CURLcode curlerr;
    
    vector<weatherData>::iterator it;
    string postData;
    bool ret;
    
    curlerr = CURLE_FAILED_INIT;
    curl = curl_easy_init();
    ret = true;
    
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, WU_UPLOAD_URL);
        
        for (it = points->begin(); it != points->end(); ++it)
        {
            postData = getWuPostString(*it);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
            
            if (curl_easy_perform(curl) != CURLE_OK)
                ret = false;
            else
                points->erase(it);
        }
        
        curl_easy_cleanup(curl);
        
        return ret;
    }
    else
        return false;
}


weatherData WeatherBase::readLastObservation (string fname)
{
    string raw;
    vector<string> line;
    weatherData buf;
    
    raw = readLastLine(fname);
    
    line = splitLogLine(raw);
    
    //** DATA ACQUISITION TIME **
    buf.dataTime = getLogTime(line.at(0));
    
    // ** TEMPERATURE **
    buf.temp = stod(splitLn.at(4));
    
    // ** DEW POINT **
    buf.dew = stod(splitLn.at(5));
    
    // ** HUMIDITY **
    buf.humidity = stoi(splitLn.at(7));
    
    // ** WIND SPEED **
    buf.windSpeed = stod(splitLn.at(8));
    
    // ** WIND GUST **
    buf.windGust = stod(splitLn.at(9));
    
    // ** WIND DIRECTION (DEGREES) **
    buf.windDeg = stod(splitLn.at(10));
    
    // ** WIND DIRECTION (STRING) **
    buf.windDir = splitLn.at(11);
    
    // ** WIND CHILL **
    buf.windChill = stod(splitLn.at(12));
    
    // ** RAIN (LAST HOUR) **
    buf.rain = stod(splitLn.at(13));
    
    // ** RELATIVE PRESSURE **
    buf.relPressure = stod(splitLn.at(16));
    
    return buf;
}

weatherData WeatherBase::parseHistlogLine (string line)
{
    weatherData buf;
    
    vector<string> splitLn;
    
    splitLn = splitLogLine(line);
    
    // ** DATA RECORDING TIME **
    buf.dataTime = getLogTime(line);
    
    // ** TEMPERATURE **
    buf.temp = stod(splitLn.at(4));
    
    // ** DEW POINT **
    buf.dew = stod(splitLn.at(5));
    
    // ** HUMIDITY **
    buf.humidity = stoi(splitLn.at(7));
    
    // ** WIND SPEED **
    buf.windSpeed = stod(splitLn.at(8));
    
    // ** WIND DIRECTION (DEGREES) **
    buf.windDeg = stod(splitLn.at(9));
    
    // ** WIND DIRECTION (STRING) **
    buf.windDir = splitLn.at(10);
    
    // ** WIND CHILL **
    buf.windChill = stod(splitLn.at(11));
    
    // ** RAIN (LAST HOUR) **
    buf.rain = stod(splitLn.at(12));
    
    // ** RELATIVE PRESSURE **
    buf.relPressure = stod(splitLn.at(15));
    
    buf.windGust = -1.0;
    
    return buf;
}

/**** PRIVATE FUNCTIONS ****/


string WeatherBase::getWuPostString (weatherData point)
{
    stringstream ss;
    
    // Station ID and password
    ss << "ID=" << m_userID << "&PASSWORD=" << m_passwd;
    
    // Observation time
    ss << "&dateutc=" << formatWuTime(point.dataTime);
    
    // Wind direction (degrees)
    ss << "&winddir=" << point.windDeg;
    
    // Wind speed (MPH)
    ss << "&windspeedmph=" << point.windSpeed;
    
    // Wind gust (MPH) if available
    if (point.windGust >= 0.0)
        ss << "&windgustmph=" << point.windGust;
    
    // Outdoor temperature
    ss << "&tempf=" << point.temp;
    
    // Dew point
    ss << "&dewptf=" << point.dew;
    
    // Rain (1 hour)
    ss << "&rainin=" << point.rain;
    
    // Relative pressure
    ss << "&baromin=" << point.relPressure;
    
    // Humidity
    ss << "&humidity=" << point.humidity;
    
    // Software recording type
    ss << "&softwaretype=open2300%20v1.11";
    
    // Append update action
    ss << "&action=updateraw";
    
    return ss.str();
}

string WeatherBase::formatWuTime (struct tm ptTm)
{
    stringstream ss;
    
    ss << (ptTm.tm_year + 1900) << '-' << (ptTm.tm_mon + 1) << '-';
    ss << (ptTm.tm_mday) << '+';
    
    if (ptTm.tm_hour < 10)
        ss << '0';
    
    ss << ptTm.tm_hour << "%3A";
    
    if (ptTm.tm_min < 10)
        ss << '0';
    
    ss << ptTm.tm_min << "%3A";
    
    if (ptTm.tm_sec)
        ss << '0';
    
    ss << ptTm.tm_sec;
    
    return ss.str();
}

struct tm WeatherBase::getLogTime (string line)
{
    struct tm buf;
    
    buf.tm_year = (stoi(line.substr(0,4)) - 1900);
    buf.tm_mon = (stoi(line.substr(4,2)) - 1);
    buf.tm_mday = stoi(line.substr(6,2));
        
    buf.tm_hour = stoi(line.substr(8,2));
    buf.tm_min = stoi(line.substr(10,2));
    buf.tm_sec = stoi(line.substr(12,2));
    
    return buf;
}

vector<string> WeatherBase::splitLogLine (string line)
{
    stringstream ss;
    vector<string> split;
    string buf;
    
    ss << line
    
    while (ss)
    {
        if (!getline(&ss, &buf, ' ')) break;
        split.push_back(buf);
    }
    
    ss.clear();
    ss.str("");
    
    return split;
}

string WeatherBase::readLastLine (string fname)
{
    ifstream fin;
    string buf;
    
    fin.open(fname.c_str());
    
    if (fin.is_open() && fin.good())
        while (getline(&fin, &buf, '\n'));
    else
        buf = "";
    
    return buf;
}