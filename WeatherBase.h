#ifndef WEATHERBASE_H
#define WEATHERBASE_H

#include <sstream>
#include <vector>
#include <ctime>
#include <string>

#include <curl/curl.h>

#define WU_UPLOAD_URL "http://weatherstation.wunderground.com/weatherstation/updateweatherstation.php"

using namespace std;

struct weatherData {
    struct tm dataTime; //TIME IS IN UTC!!!
    double temp;
    double dew;
    int humidity;
    double windSpeed;
    double windGust;
    double windDeg;
    string windDir;
    double windChill;
    double rain;
    double relPressure;
};

class WeatherBase {
public:
    
    /**
     * @brief WeatherBase: class constructor
     * @param userID: the ID of the weather station
     * @param passwd: the password to the account
     */ 
    WeatherBase (string userID, string passwd);
    
    /**
     * @brief ~WeatherBase: class destructor
     */ 
    ~WeatherBase();
    
    /**
     * @brief ~WeatherBase: class destructor
     * 
     * Currently does nothing.
     */ 
    virtual ~WeatherBase() {}
    
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
      
      /**
       * @brief readLastObservation: grab the last observation from a run2300d log file
       * @param fname: the path to the run2300d log file
       * @return: a weatherData struct representing the data point
       */ 
      weatherData readLogData(string fname);
      
      /**
       * @brief parseHistlogLine: parse information from the station's internal
       *        log
       * @param line: the histlog line to parse
       * @return: a weatherData struct representing the data point
       */ 
      weatherData parseHistlogLine (string line);
      
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
      
    /**
     * @brief getLogTime: parse an observation time
     * @param line: the log line (run2300d/histlog) to parse the time from
     * @return: a tm struct containing the observation time
     */ 
    struct tm getLogTime (string line);
      
    /**
     * @brief splitLogLine: split a log line apart by spaces
     * @param line: the log line to split
     * @return: a vector containing string elements of the line (order preserved)
     */
    vector<string> splitLogLine (string line);
      
    /**
     * @brief readLastLine: read the last line in a file
     * @param fname: the file to read from
     * @return: the last line of the file
     */ 
    string readLastLine (string fname);
      
    string m_userID, m_passwd;
};

#endif //WEATHERBASE_H