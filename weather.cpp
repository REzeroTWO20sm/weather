/*Ниже приведен пример кода на C++ с использованием библиотек curl и nlohmann для работы с API OpenWeather. Код реализует функции для выполнения заданных операций:*/

#include <chrono>
#include <ctime>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

//g++ weather.cpp -o weather --std=c++14 -I/usr/local/include -I/usr/include/nlohmann -lcurl

// Функция обратного вызова для записи данных от curl в std::string
size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char *)contents, newLength);
    } catch (std::bad_alloc &e) {
        // Обработка ошибок при нехватке памяти
        return 0;
    }
    return newLength;
}

int getDtForDate(int year, int month, int day) {
    int buffer;
    std::tm timeinfo = {0};
    timeinfo.tm_year = year - 1900; // years since 1900
    timeinfo.tm_mon = month - 1; // months since January [0-11]
    timeinfo.tm_mday = day; // day of the month [1-31]

    time_t time_since_epoch = std::mktime(&timeinfo);
    buffer = time_since_epoch;
    cout << endl << to_string(buffer) << endl ;
    return buffer;
}

pair<std::string, std::string> take_cords(string address){
    CURL *curl;
    CURLcode res;
    string readBuffer;
    stringstream lat_ss;
    stringstream lon_ss;
    float lat_f;
    float lon_f;
    string lat;
    string lon;
    string url;
    curl = curl_easy_init();
    if(curl){
        //"https://catalog.api.2gis.com/3.0/items/geocode?q=Москва, Садовническая, 25&fields=items.point&key=0038d200d-62ed-4147-8a11-b996a7e2c7a7
        url = "https://catalog.api.2gis.com/3.0/items/geocode?q=" + address + "&fields=items.point&key=0038d200-62ed-4147-8a11-b996a7e2c7a7";
        /* string url = "https://geocode-maps.yandex.ru/1.x/?apikey=dbcd5d6e-f034-4c9e-a84d-11c201dbd4d4&lang=ru_RU&geocode="+ city +"&format=json"; определение station города*/
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res == CURLE_OK){
            json response = json::parse(readBuffer);
            lat_f = response["result"]["items"][0]["point"]["lat"];
            lon_f = response["result"]["items"][0]["point"]["lon"];
            lat_ss << lat_f;
            lon_ss << lon_f;
            lat  = lat_ss.str();
            lon = lon_ss.str();
            /*pos = pos.replace(pos.find(" "),1,"&lat=");*/                                                                                                                        //37.617698&lan=55.755864
            
        }
        else {
            std::cerr << "Ошибка при выполнении запроса: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return make_pair(lat, lon);
}
// Функция получения прогноза для указанного почтового индекса
json getWeatherForecastByZip(string &apiKey,string &ZipKey) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    json response;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "http://api.openweathermap.org/data/2.5/weather?&zip="+ZipKey+"&appid=" + apiKey;//http://api.openweathermap.org/data/2.5/weather?&zip=94040&appid=1ff15930de8f02537dc1d994d0ad603e
        // "http://api.openweathermap.org/data/2.5/weather?&lat="+lat+"&lon="+lon+"&appid=1ff15930de8f02537dc1d994d0ad603e"
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            response = json::parse(readBuffer);
            readBuffer.clear();
        } else {
            std::cerr << "Ошибка при выполнении запроса: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return response;
}

// Функция получения прогноза на все 5 дней
json getFiveDayWeatherForecast(const std::string &apiKey, string lat , string lon) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    json response;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "http://api.openweathermap.org/data/2.5/forecast?&lat="+lat+"&lon="+lon+"&appid=" + apiKey;
        cout << endl << url << endl;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            response = json::parse(readBuffer);
            readBuffer.clear();
        } else {
            std::cerr << "Ошибка при выполнении запроса: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return response;
}

// Функция получения прогноза на конкретный день
json getWeatherForecastForDay(const std::string &apiKey, string lat , string lon , int dt) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    json response;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "http://api.openweathermap.org/data/2.5/forecast?&lat=" + lat + "&lon=" + lon + "&appid=" + apiKey;
        cout << endl << url << endl;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            response = json::parse(readBuffer);
            readBuffer.clear();
        } else {
            std::cerr << "Ошибка при выполнении запроса: " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
    return response;
}

// Функция получения самой низкой и самой высокой температуры из всех прогнозов
void getMinMaxTemperature(const json &forecastData, double &minTemp, double &maxTemp) {
    minTemp = maxTemp = forecastData["list"][0]["main"]["temp"];
    // for (const auto &entry : forecastData["list"]) {
        
    // }
    int i=0;
    while(!forecastData["list"][i].is_null()){
        double temp = forecastData["list"][i]["main"]["temp"];
        if (temp < minTemp) {
            minTemp = temp;
        }
        if (temp > maxTemp) {
            maxTemp = temp;
        }
        i+=1;
    }
}

int main() {

    int year = 2024;
    int mounth = 1;
    int day = 28;
    int dt = getDtForDate(year,mounth,day);
    // string dt_str = ctime(&dt);
    dt = dt + 75600 - 64800;
    
    pair<string, string> result;
    string address;
    string lat;
    string lon;
    address = "москва";
    result = take_cords(address);
    lat = result.first;
    lon = result.second;
    cout << endl << lat << " " << lon << endl;
    std::string apiKey = "1ff15930de8f02537dc1d994d0ad603e"; // Вставьте ваш API-ключ
    std::string zipCode = "94040"; // Пример: почтовый индекс
    json currentWeather = getWeatherForecastByZip(apiKey,zipCode);
    cout << currentWeather["weather"][0]["description"] << endl 
    << currentWeather["main"]["temp"] << endl
    << currentWeather["wind"]["speed"] << endl;

    json fiveDayForecast = getFiveDayWeatherForecast(apiKey, lat,lon);
    int i=0;
    while(!fiveDayForecast["list"][i].is_null()){
        cout << endl << fiveDayForecast["list"][i]["main"]["temp"] << " , "
        << fiveDayForecast["list"][i]["weather"][0]["description"] << " , "
        << fiveDayForecast["list"][i]["dt_txt"] << endl;
        i+=1;
    }
    i=0;
    json WeatherForDay = getWeatherForecastForDay(apiKey, lat,lon, dt);
    while (WeatherForDay["list"][i]["dt"] >= dt && WeatherForDay["list"][i]["dt"]<dt+86400){
        // if((dt+43200) <= WeatherForDay["list"][i]["dt"] >= dt-43200){
        cout << endl<<"99"<<endl << WeatherForDay["list"][i]["main"]["temp"] << " , "
        << WeatherForDay["list"][i]["weather"][0]["description"] << " , "
        << WeatherForDay["list"][i]["dt_txt"] << " , "
        << WeatherForDay["list"][i]["dt"] << endl;
        i+=1;
    }

    double minTemp, maxTemp;
    minTemp=1000;
    maxTemp=-1000;
    getMinMaxTemperature(fiveDayForecast, minTemp, maxTemp);
    std::cout << endl <<"Самая низкая температура: " << minTemp << "K" << std::endl;
    std::cout << "Самая высокая температура: " << maxTemp << "K" << std::endl;
    
    return 0;
}