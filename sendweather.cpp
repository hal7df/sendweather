#include <iostream>
#include <cstdlib>

#include <curl/curl.h>

#include "WeatherUpdater.h"
using namespace std;

int main (int argc, char* argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);
    
    WeatherUploader* uploader;
    
    uploader = new WeatherUploader (/* GET USERID AND PASSWORD FROM CONFIGURATION LATER*/);
    
    cout << "Determining last update...\t\t\t"
    
    if (!uploader->fetchLastUpdate())
    {
        cout << "[ FAILED ]" << endl;
        exit(EXIT_FAILURE);
    }
    
    cout << "[ Done ]" << endl << "Uploading unattended weather data...\t\t\t";
    cout << endl;
    
    if (!uploader->uploadUnattended())
    {
        cout << "[ FAILED ]" << endl;
        exit(EXIT_FAILURE);
    }
    
    cout << "[ Done ]" << endl;
    
    curl_global_cleanup();
    
    return 0;
}