#include "WeatherUpdater.h"

/** CONSTRUCTOR **/
WeatherUpdater::WeatherUpdater (string fname, string userID, string passwd)
{
    m_fname = fname;
    m_userID = userID;
    m_passwd = passwd;
    
    m_logreader = new LogReader;
    
    curl_global_init(CURL_GLOBAL_ALL);
}

/** DESTRUCTOR **/
WeatherUpdater::~WeatherUpdater ()
{
    curl_global_cleanup();
}

/** PUBLIC FUNCTIONS **/

bool WeatherUpdater::fetchLastUpdate ()
{
    if (!downloadHistoryFile())
        return false;
    else
    {
        findLastUpdate();
        readUnattendedData();
        return true;
    }
}

bool WeatherUpdater::uploadUnattended ()
{
    CURL* curl;
    CURLcode curlerr;
    
    vector<weatherData>::iterator it;
    string postData;
    bool ret;
    
    ret = true;
    
    curl = curl_easy_init();
    
    if (curl)
    {
        ret = true;
        
        curl_easy_setopt(curl, CURLOPT_URL, WU_UPLOAD_URL);
        
        for (it = m_unattendedData.begin(); it != m_unattendedData.end(); ++it)
        {
            postData = getWuPostString(*it);
            
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str());
            
            curlerr = curl_easy_perform(curl);
            
            if (curlerr != CURLE_OK)
                ret = false;
        }
        
        curl_easy_cleanup(curl);
        
        return ret;
    }
    else
        return false;
}

CURLcode WeatherUpdater::uploadWeatherData (weatherData point)
{
    CURL* curl
    CURLcode curlerr;
    
    string postData;
    bool ret;
   
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

size_t WeatherUpdater::writeData (void *data, size_t size, size_t nmemb, FILE *stream)
{
    return fwrite(data, size, nmemb, stream);
}

/** INTERNAL FUNCTIONS **/

//TODO: Check if file is empty (no data points)
bool WeatherUpdater::downloadHistoryFile()
{
    CURL* curl;
    CURLcode res;
    FILE* dl;
    string dl_url;
    
    stringstream ss (stringstream::in | stringstream::out);
    time_t* rawtime;
    struct tm* tm_info;
    
    time(rawtime);
    tm_info = localtime(rawtime);
    
    curl = curl_easy_init();
    
    if (curl)
    {
        int x;
        
        x = 0;
        do 
        {
            ss << "http://www.wunderground.com/weatherstation/WXDailyHistory.asp?";
            ss << "?ID=" << m_userID << "&day=" << (tm_info.tm_mday + x);
            ss << "&month=" << (tm_info.tm_mon + 1);
            ss << "&year=" << (tm_info.tm_year + 1900) << "&graphspan=day";
            ss << "&format=1";
            
            ss >> dl_url;
            
            ss.clear();
            ss.str("");
            
            dl = fopen("/tmp/weatherlog.csv","w");
            
            curl_easy_setopt(curl, CURLOPT_URL, dl_url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WeatherUpdater::writeData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, dl);
            res = curl_easy_perform(curl);
            
            fclose(dl);
            x--;
        } while (res != CURLE_OK && (((tm_info.tm_mday + x) > 0) && (x > -8)));
        
        curl_easy_cleanup(curl);
        
        if (res != CURLE_OK)
            return false;
        else
        {
            if (x < -7)
                f_uploadWeek = true;
                
            return true;
        }
    }
    else
    {
        curl_easy_cleanup(curl);
        return false;
    }
}

void WeatherUpdater::findLastUpdate ()
{
    if (f_uploadWeek)
    {
        time_t curTime;
        
        time(&curTime);
        
        curTime -= 604800; //604800 = seconds in a week
        
        m_lastUpdate = localtime(&curTime);
    }
    else
        m_lastUpdate = m_logreader->GetLastWuUpdate("/tmp/weatherlog.csv");
}

void WeatherUpdater::readUnattendedData ()
{
    ifstream fin;
    string rawline;
    struct tm ptTm;
    bool startFlag;
    
    weatherData buf;
    
    startFlag = false;
    
    fin.open(m_fname.c_str());
    
    do
    {
        getline(&fin, &rawline, '\n');
        
        buf = m_logreader->parseHistlogLine(rawline);
        
        /* ! WARNING ! The following two if statements CANNOT be changed to
         * an if-else structure: the second if must run if the first changes
         * the state of startFlag
         */
        
        if (!startFlag)
        {
            if (mktime(&(buf.dataTime)) >= mktime(&m_lastUpdate))
                startFlag = true;
        }
        
        if (startFlag)
            m_unattendedData.push_back(buf);
            
    } while (fin.good());
}

string WeatherUpdater::getWuPostString (weatherData point)
{
    stringstream ss (stringstream::in | stringstream::out);
    
    // Station ID and password
    ss << "ID=" << m_userID << "&PASSWORD=" << m_passwd;
    
    // Observation time
    ss << "&dateutc=" << formatWuTime(point.dataTime);
    
    // Wind direction (degrees)
    ss << "&winddir=" << point.windDeg;
    
    // Wind speed (MPH)
    ss << "&windspeedmph=" << point.windSpeed;
    
    // Wind gust (MPH)
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
    ss << "&softwaretype=open2300%20version1.11";
    
    // Append update action
    ss << "&action=updateraw";
    
    return ss.str();
}

string WeatherUpdater::formatWuTime (struct tm ptTm)
{
    stringstream ss (stringstream::in | stringstream::out);
    time_t pointTime;
    
    pointTime = mktime(&ptTm);
    ptTm = gmtime(&pointTime);
    
    
    ss << (point.dataTime.tm_year + 1900) << '-' << (point.dataTime.tm_mon + 1);
    ss << '-' << point.dataTime.tm_mday << '+';
    
    if (point.dataTime.tm_hour < 10)
        ss << '0' << point.dataTime.tm_hour << "%3A";
    else
        ss << point.dataTime.tm_hour << "%3A";
        
    if (point.dataTime.tm_min < 10)
        ss < '0' << point.dataTime.tm_min << "%3A";
    else
        ss << point.dataTime.tm_min << "%3A";
        
    if (point.dataTime.tm_sec < 10)
        ss << '0' << point.dataTime.tm_sec;
    else
        ss << point.dataTime.tm_sec;
        
    return ss.str();
}