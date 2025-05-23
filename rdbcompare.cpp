#include "rdbcompare.hpp"
#include <curl/curl.h>
#include <string>
#include <cstring>
#include <iostream>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* data) {
    size_t total_size = size * nmemb;
    data->append(static_cast<char*>(contents), total_size);
    return total_size;
}

extern "C" {

char* fetch_package_list(const char* branch) {

    if (!branch || strlen(branch) == 0) { 
        std::cerr << "Error: Invalid branch name" << std::endl;
        return nullptr;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Error: curl_easy_init() failed" << std::endl;
        return nullptr;
    }

    std::string url = "https://rdb.altlinux.org/api/export/branch_binary_packages/" + std::string(branch);

    std::string response_data;//Data buffer


    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "rdbcompare/1.0");


    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Error: curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
        return nullptr;
    }

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    if (http_code != 200) {
        std::cerr << "Error: HTTP response code " << http_code << std::endl;
        return nullptr;
    }

    char* result = strdup(response_data.c_str());
    if (!result) {
        std::cerr << "Error: strdup() failed" << std::endl;
        return nullptr;
    }

    return result;
}

char* compare_packages(const char* branch1_data, const char* branch2_data) {
    // Заглушка
    std::string result = R"({
        "architectures": {}
    })";
    char* ret = strdup(result.c_str());
    return ret;
}

}