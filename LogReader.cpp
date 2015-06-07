LogReader::LogReader()
{
    
}

struct tm LogReader::getLastWuUpdate (string fname)
{
    string raw;
    struct tm update;
    
    raw = readLastLine(fname);
    
    if (raw != "")
    {
        raw = raw.substr(0,19);
        
        update.tm_year = (stoi(raw.substr(0,4)) - 1900);
        update.tm_mon = (stoi(raw.substr(5,2)) - 1);
        update.tm_mday = stoi(raw.substr(8,2));
            
        update.tm_hour = stoi(raw.substr(11,2));
        update.tm_min = stoi(raw.substr(14,2));
        update.tm_sec = stoi(raw.substr(17,2));
        
        remove(fname.c_str());
    }
    else
        update.tm_year = -1;
        
    return update;
}

weatherData LogReader::parseLogLine (string line)
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

weatherData LogReader::parseHistlogLine (string line)
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

struct tm LogReader::getLogTime (string line)
{
    struct tm buf;
    
    buf.tm_year = (stoi(line.substr(0,4)) - 1900);
    buf.tm_mon = (stoi(line.substr(4,2)) - 1);
    buf.tm_mday = stoi(line.substr(6,2));
        
    buf.tm_hour = stoi(line.substr(8,2));
    buf.tm_min = stoi(line.substr(10,2));
    buf.tm_sec = stoi(line.substr(12,2));
}

vector<string> LogReader::splitLogLine (string line)
{
    stringstream ss (stringstream::in | stringstream::out);
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

string LogReader::readLastLine (string fname)
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