#ifndef WUTILS_H
#define WUTILS_H

#include <sstream>
#include <vector>
#include <ctime>
#include <string>

#include <curl/curl.h>

#include "WeatherData.h"

#define WU_UPLOAD_URL "http://weatherstation.wunderground.com/weatherstation/updateweatherstation.php"

using namespace std;

class WuUpload {
public:
    
    /**
     * @brief WuUpload: class constructor
     * @param userID: the ID of the weather station
     * @param passwd: the password to the account
     */ 
    WuUpload (string userID, string passwd);
    
    /**
     * @brief ~WuUpload: class destructor
     * 
     * Currently does nothing.
     */ 
    virtual ~WuUpload() {}
    
    /**
     * @brief uploadWeatherData: upload a single weather observation
     * 
     *      This function should only be used for single points. To upload 
     *      multiple weather observations in bulk, please use bulkUpload().
     * 
     * @param point: The weatherData struct representing the observation.
     * @return: the CURLcode returned from the operation.
     */
     CURLcode uploadWeatherData(weatherData point);
     
     /**
      * @brief bulkUpload: upload multiple weather observations
      * @param points: A vector of weatherData structs representing all of the 
      *             desired weather observations to be uploaded.
      * @return: True if all uploads succeeded, false otherwise
      */
      bool bulkUpload(vector<weatherData> points);
      
private:
    
    /**
     * @brief getWuPostString: convert a weatherData struct to a POST string
     *                          for Weather Underground.
     * @param point: the weatherData struct representing the observation.
     * @return: A string containing all of the POST data.
     */
     string getWuPostString(weatherData point);
     
     /**
      * @brief formatWuTime: convert a local tm structure to Weather
      *                       Underground-format UTC time.
      * @param ptTm: the tm structure of the data acquisition time.
      * @return: a string in percent-encoded WU-style time.
      */
      string formatWuTime (struct tm ptTm);
      
      string m_userID, m_passwd;
};

#endif //WUTILS_H