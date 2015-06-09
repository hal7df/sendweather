#include "WuUpload.h"

WuUpload::WuUpload (string userID, string passwd)
{
    m_userID = userID;
    m_passwd = passwd;
}

CURLcode WuUpload::uploadWeatherData (weatherData point)
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

bool WuUpload::bulkUpload (vector<weatherData> points)
{
    
}