#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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

// Функция получения прогноза для указанного почтового индекса
json getWeatherForecastByZip(const std::string &apiKey, const std::string &zipCode) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    json response;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "http://api.openweathermap.org/data/2.5/weather?zip=" + zipCode + "&appid=" + apiKey;
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
json getFiveDayWeatherForecast(const std::string &apiKey, const std::string &zipCode) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    json response;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "http://api.openweathermap.org/data/2.5/forecast?zip=" + zipCode + "&appid=" + apiKey;
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
json getWeatherForecastForDay(const std::string &apiKey, const std::string &zipCode, int day) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    json response;

    curl = curl_easy_init();
    if (curl) {
        std::string url = "http://api.openweathermap.org/data/2.5/forecast?zip=" + zipCode + "&appid=" + apiKey;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            json forecast = json::parse(readBuffer);
            if (forecast.find("list") != forecast.end() && forecast["list"].size() > day) {
                response = forecast["list"][day];
            } else {
                std::cerr << "Прогноз на указанный день отсутствует." << std::endl;
            }
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
    for (const auto &entry : forecastData["list"]) {
        double temp = entry["main"]["temp"];
        if (temp < minTemp) {
            minTemp = temp;
        }
        if (temp > maxTemp) {
            maxTemp = temp;
        }
    }
}

int main() {
    std::string apiKey = "1ff15930de8f02537dc1d994d0ad603e"; // Вставьте ваш API-ключ
    std::string zipCode = "94040"; // Пример: почтовый индекс

    json currentWeather = getWeatherForecastByZip(apiKey, zipCode);
    std::cout << "Текущий прогноз погоды: " << currentWeather.dump(4) << std::endl;

    json fiveDayForecast = getFiveDayWeatherForecast(apiKey, zipCode);
    std::cout << "Прогноз на 5 дней: " << fiveDayForecast.dump(4) << std::endl;

    int specificDay = 2; // Пример: прогноз на третий день
    json specificDayForecast = getWeatherForecastForDay(apiKey, zipCode, specificDay);
    std::cout << "Прогноз на " << specificDay << " день: " << specificDayForecast.dump(4) << std::endl;

    double minTemp, maxTemp;
    getMinMaxTemperature(fiveDayForecast, minTemp, maxTemp);
    std::cout << "Самая низкая температура: " << minTemp << "K" << std::endl;
    std::cout << "Самая высокая температура: " << maxTemp << "K" << std::endl;

    return 0;
}
