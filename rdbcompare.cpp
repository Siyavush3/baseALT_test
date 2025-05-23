#include "rdbcompare.hpp"
#include <string>

extern "C" {

char* fetch_package_list(const char* branch) {
    // Заглушка
    std::string result = "{\"packages\": []}"; 
    char* ret = strdup(result.c_str());
    return ret;
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