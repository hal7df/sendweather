#include "WeatherDaemon.h"

WeatherDaemon::WeatherDaemon (string userID, string passwd, string fname, string config)
{
    m_fname = fname;
    m_config = config;
    m_userID = userID;
    m_passwd = passwd;
    
    m_logreader = new LogReader;
    
    m_daemonError = SENDWEATHERD_NOERROR;
    
    f_endDaemon = false;
}

WeatherDaemon::~WeatherDaemon()
{
    if (m_awaitingUpload.size() > 0)
    {
        vector<weatherData>::iterator it;
        ofstream fout;
        stringstream ss;
        
        ss << m_config << "/daemon-awaiting-upload.txt";
        
        fout.open(ss.str().c_str());
        
        for (it = m_awaitingUpload.begin(); it != m_awaitingUpload.end(); ++it)
        {
            fout << mktime(&((*it).dataTime)) << ' ' << (*it).temp << ' ';
            fout << (*it).dew << ' ' << (*it).humidity << ' ';
            fout << (*it).windSpeed << ' ' << (*it).windGust << ' ';
            fout << (*it).windDeg << ' ' << (*it).windDir << ' ';
            fout << (*it).windChill << ' ' << (*it).rain << ' ';
            fout << (*it).relPressure << endl;
        }
    }
}

bool WeatherDaemon::runDaemon ()
{
    string raw;
    weatherData buf;
    vector<weatherData>::iterator it;
    
    InotifyEvent* event;
    size_t eventCount;
    
    f_endDaemon = false;
    
    try
    {
        Inotify notify;
        InotifyWatch watch (getLogPath(), IN_MODIFY);
        
        notify.Add(watch);
        
        do
        {
            notify.WaitForEvents();
            
            eventCount = notify.GetEventCount();
            
            while (eventCount > 0)
            {
                if (notify.GetEvent(event))
                {
                    if ((event->GetName()).find(getLogFilename()) != string::npos)
                    {
                        for (it = m_awaitingUpload.begin(); it != m_awaitingUpload.end(); ++it)
                        {
                            if (uploadWeatherData(*it) == CURLE_OK)
                                m_awaitingUpload.erase(it);
                        }
                        
                        if (m_awaitingUpload.size() == 0 && m_daemonError == SENDWEATHERD_POSTERROR)
                            m_daemonError = SENDWEATHERD_NOERROR;
                        
                        raw = m_logreader->readLastLine(m_fname);
                        buf = m_logreader->parseLogLine(raw);
                        
                        if (uploadWeatherData(buf) != CURLE_OK)
                        {
                            m_awaitingUpload.push_back(buf);
                            m_daemonError = SENDWEATHERD_POSTERROR;
                        }
                    }
                }
            }
        } while (m_daemonError < SENDWEATHERD_FATALERROR && !f_endDaemon);
    }
    catch (InotifyException& e)
    {
        cerr << "File watch exception: " << e.GetMessage() << endl;
        m_daemonError = SENDWEATHERD_WATCHERROR;
    }
    catch (exception& e)
    {
        cerr << "Standard exception: " << e.what() << endl;
        m_daemonError = SENDWEATHERD_STDERROR;
    }
    catch (...)
    {
        cerr << "Unknown exception" << endl
        m_daemonError = SENDWEATHERD_UNKERROR;
    }
}

bool WeatherDaemon::uploadCachedData()
{
    ifstream fin;
    stringstream ss;
    
    weatherData buf;
    time_t timeBuf;
    
    vector<weatherData>::iterator it;
    bool success;
    
    success = true;
    
    ss << m_config << "/daemon-awaiting-upload.txt";
    
    fin.open(ss.str().c_str());
    
    while (fin.good())
    {
        fin >> timeBuf >> buf.temp >> buf.dew >> buf.humidity;
        fin >> buf.windSpeed >> buf.windGust >> buf.windDeg;
        fin >> buf.windDir >> buf.windChill >> buf.rain >> buf.relPressure;
        
        buf.dataTime = localtime(&timeBuf);
        
        m_awaitingUpload.push_back(buf);
    }
    
    for (it = m_awaitingUpload.begin(); it != m_awaitingUpload.end(); ++it)
    {
        if (uploadWeatherData(*it) != CURLE_OK)
            success = false;
    }
    
    return success;
}

string WeatherDaemon::getLogFilename()
{
    return m_fname.substr((m_fname.find_last_of('/')) + 1);
}

string WeatherDaemon::getLogPath()
{
    size_t pos;
    
    pos = m_fname.find_last_of('/');
    
    return m_fname.substr(0,);
}