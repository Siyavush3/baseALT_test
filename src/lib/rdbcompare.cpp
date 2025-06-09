#include "rdbcompare.hpp"
#include <curl/curl.h>
#include <string>
#include <cstring>
#include <json-c/json.h>
#include <iostream>
#include <vector>
#include <memory> 
#include <mutex> 
#include <map>
#include <set> 
#include <rpm/rpmvercmp.h>

namespace rdbcompare{

    struct Package { //Вспомогательная структура
        std::string name;
        std::string epoch;
        std::string version;
        std::string release;
        std::string arch;
        Package() : name(""), epoch("0"), version(""), release(""), arch("") {}
        Package(std::string n, std::string e, std::string v, std::string r, std::string a)
            : name(std::move(n)), epoch(std::move(e)), version(std::move(v)), release(std::move(r)), arch(std::move(a)) {}


        std::string toString() const {
            return name + "-" + version + "-" + release + "." + arch;//Для отладки
        }
    };

    using ArchPackages = std::map<std::string, std::map<std::string, Package>>;// Пакеты сгруппированные по архитектуре

    


    std::once_flag branches_init_flag;//Флаг инициализации

    // Кэш для действительных имен веток
    std::vector<std::string> cached_branches;
    size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* output) { 
        // Записывает данные HTTP-ответа в строку
        size_t total_size = size * nmemb;
        output->append(static_cast<char*>(contents), total_size);
        return total_size;
    }

    bool perform_http_request(const std::string& url, std::string& response, long& http_code) {
        //Выполняем запрос и возвращаем true при response 200
        CURL* curl = curl_easy_init();
        if (!curl) {
            std::cerr << "Error: Failed to initialize curl" << std::endl;
            return false;
        }

        auto cleanup = [](CURL* c) { curl_easy_cleanup(c); };
        std::unique_ptr<CURL, decltype(cleanup)> curl_guard(curl, cleanup);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "rdbcompare/1.0");

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Error: HTTP request failed: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        return http_code == 200;
    }

    bool is_valid_branch(const char* branch_name) {
        // Проверяет, является ли имя ветки действительным, получая и кэшируя список веток

        if (!branch_name || !*branch_name) { // Проверяет корректность входного имени ветки
            std::cerr << "Error: Invalid branch name" << std::endl;
            return false;
        }

        // Используем std::call_once для потокобезопасной инициализации cached_branches
        std::call_once(branches_init_flag, []() {
            // Запрашиваем JSON со списком веток
            std::string response;
            long http_code = 0;
            if (!perform_http_request("https://rdb.altlinux.org/api/export/branch_tree", response, http_code)) {
                std::cerr << "Error: Failed to fetch branch list, HTTP code: " << http_code << std::endl;
                return; // Возвращаемся из лямбды
            }

            json_object* parsed_json = json_tokener_parse(response.c_str()); // Парсим JSON-объект
            if (!parsed_json) {
                std::cerr << "Error: Failed to parse branch list JSON" << std::endl;
                return;
            }

            // Используем std::unique_ptr для автоматической очистки json_object
            auto cleanup_json = [](json_object* obj) { json_object_put(obj); };
            std::unique_ptr<json_object, decltype(cleanup_json)> json_guard(parsed_json, cleanup_json);

            json_object* branches; // Получаем список веток
            if (!json_object_object_get_ex(parsed_json, "branches", &branches) || !json_object_is_type(branches, json_type_array)) {
                std::cerr << "Error: Invalid branch list format" << std::endl;
                return;
            }

            // Заполняем кэш именами веток
            for (size_t i = 0; i < json_object_array_length(branches); i++) {
                const char* name = json_object_get_string(json_object_array_get_idx(branches, i));
                if (name) {
                    cached_branches.emplace_back(name);
                }
            }
        });


        if (cached_branches.empty()) {
            std::cerr << "Error: Кэш списка веток пуст. Не удалось получить ветки." << std::endl;
            return false;
        }

        // Ищем ветку в кэшированном списке
        for (const auto& name : cached_branches) {
            if (name == branch_name) {
                return true;
            }
        }
        
        std::cerr << "Error: Ветка '" << branch_name << "' не найдена в списке веток" << std::endl;
        return false;
    }
    std::string make_package_url(const char* branch_name) {
        // Формирует URL для запроса пакетов ветки
        return "https://rdb.altlinux.org/api/export/branch_binary_packages/" + std::string(branch_name);
    }

    char* allocate_result(const std::string& data) {
        // Выделяет память для результата 
        char* result = strdup(data.c_str());
        if (!result) {
            std::cerr << "Error: Failed to allocate memory for result" << std::endl;
        }
        return result;
    }


    ArchPackages parse_packages_json(const char* json_data) {
        ArchPackages arch_packages; 

        if (!json_data) {
            std::cerr << "Error: Input JSON data is null." << std::endl;
            return arch_packages;
        }

        json_object* parsed_json = json_tokener_parse(json_data);
        if (!parsed_json) {
            std::cerr << "Error: Failed to parse package list JSON. Invalid JSON format." << std::endl;
            return arch_packages;
        }

        
        auto cleanup_json = [](json_object* obj) { json_object_put(obj); };
        std::unique_ptr<json_object, decltype(cleanup_json)> json_guard(parsed_json, cleanup_json);

        json_object* packages_array;
        
        if (!json_object_object_get_ex(parsed_json, "packages", &packages_array) || !json_object_is_type(packages_array, json_type_array)) {
            std::cerr << "Error: 'packages' array not found or is not an array in JSON response." << std::endl;
            return arch_packages;
        }

        
        for (size_t i = 0; i < json_object_array_length(packages_array); ++i) {
            json_object* pkg_obj = json_object_array_get_idx(packages_array, i);
            if (!pkg_obj) {
                std::cerr << "Warning: Null package object found in array at index " << i << ". Skipping." << std::endl;
                continue;
            }

            // Временные указатели для извлечения значений
            json_object *name_obj, *epoch_obj, *version_obj, *release_obj, *arch_obj;

            if (json_object_object_get_ex(pkg_obj, "name", &name_obj) &&
                json_object_object_get_ex(pkg_obj, "epoch", &epoch_obj) &&
                json_object_object_get_ex(pkg_obj, "version", &version_obj) &&
                json_object_object_get_ex(pkg_obj, "release", &release_obj) &&
                json_object_object_get_ex(pkg_obj, "arch", &arch_obj))
            {
                
                std::string epoch_str = (epoch_obj && json_object_get_string(epoch_obj)) ? json_object_get_string(epoch_obj) : "0";

                
                Package pkg(
                    json_object_get_string(name_obj),
                    epoch_str,
                    json_object_get_string(version_obj),
                    json_object_get_string(release_obj),
                    json_object_get_string(arch_obj)
                );

                arch_packages[pkg.arch][pkg.name] = pkg;
            } else {
                std::cerr << "Warning: Missing one or more required fields (name, epoch, version, release, arch) for package at index " << i << ". Skipping." << std::endl;
            }
        }

        return arch_packages;
    }
    // Возвращает: >0 если pkg1 новее, <0 если pkg2 новее, 0 если равны.
    int compare_versions(const Package& pkg1, const Package& pkg2) {
        long epoch1 = std::atol(pkg1.epoch.c_str());
        long epoch2 = std::atol(pkg2.epoch.c_str());

        if (epoch1 != epoch2) {
            return epoch1 - epoch2;
        }

        int ver_cmp_result = rpmvercmp(pkg1.version.c_str(), pkg2.version.c_str());

        if (ver_cmp_result != 0) {
            return ver_cmp_result;
        }

        int rel_cmp_result = rpmvercmp(pkg1.release.c_str(), pkg2.release.c_str());

        return rel_cmp_result;
    }
}
extern "C" {
    void rdbcompare_init() {
        curl_global_init(CURL_GLOBAL_ALL);
    }

    void rdbcompare_cleanup() {
        curl_global_cleanup();
    }

    char* fetch_package_list(const char* branch) {

        if (!rdbcompare::is_valid_branch(branch)) {
            return nullptr;
        }

        std::string response;
        long http_code = 0;
        std::string url = rdbcompare::make_package_url(branch);

        if (!rdbcompare::perform_http_request(url, response, http_code)) {
            std::cerr << "Error: Failed to fetch packages for '" << branch << "', HTTP code: " << http_code << std::endl;
            return nullptr;
        }

        return rdbcompare::allocate_result(response);

    }

char* compare_packages(const char* branch1_data, const char* branch2_data) {

    if (!branch1_data || !branch2_data) {
        std::cerr << "Error: One or both branch data inputs are null." << std::endl;
        return nullptr;
    }


    rdbcompare::ArchPackages branch1_pkgs = rdbcompare::parse_packages_json(branch1_data);
    rdbcompare::ArchPackages branch2_pkgs = rdbcompare::parse_packages_json(branch2_data);

    if (branch1_pkgs.empty() && strlen(branch1_data) > 0) {
        std::cerr << "Error: Failed to parse packages for branch 1." << std::endl;
        return nullptr;
    }
    if (branch2_pkgs.empty() && strlen(branch2_data) > 0) {
        std::cerr << "Error: Failed to parse packages for branch 2." << std::endl;
        return nullptr;
    }


    json_object* result_json = json_object_new_object();
    auto cleanup_result_json = [](json_object* obj) { json_object_put(obj); };
    std::unique_ptr<json_object, decltype(cleanup_result_json)> result_guard(result_json, cleanup_result_json);

    json_object* architectures_json = json_object_new_object();
    json_object_object_add(result_json, "architectures", architectures_json);

    // Инициализация глобальных счетчиков для раздела "summary"
    int total_branch1_only_count = 0;
    int total_branch2_only_count = 0;
    int total_branch1_newer_count = 0;

    std::set<std::string> all_architectures;

    for (const auto& pair : branch1_pkgs) {
        all_architectures.insert(pair.first);
    }

    for (const auto& pair : branch2_pkgs) {
        all_architectures.insert(pair.first);
    }


    for (const std::string& arch : all_architectures) {
        json_object* arch_comparison_json = json_object_new_object();
        json_object_object_add(architectures_json, arch.c_str(), arch_comparison_json);

        // Создаем объекты для каждой категории, которые будут содержать count и packages
        json_object* branch1_only_obj = json_object_new_object();
        json_object* branch2_only_obj = json_object_new_object();
        json_object* branch1_newer_obj = json_object_new_object();

        // Временные счетчики для текущей архитектуры
        int arch_branch1_only_count = 0;
        int arch_branch2_only_count = 0;
        int arch_branch1_newer_count = 0;

        // Создаем массивы, в которые будут добавляться пакеты
        json_object* branch1_only_packages_array = json_object_new_array();
        json_object* branch2_only_packages_array = json_object_new_array();
        json_object* branch1_newer_packages_array = json_object_new_array();

        // Добавляем эти массивы в соответствующие объекты категорий
        json_object_object_add(branch1_only_obj, "packages", branch1_only_packages_array);
        json_object_object_add(branch2_only_obj, "packages", branch2_only_packages_array);
        json_object_object_add(branch1_newer_obj, "packages", branch1_newer_packages_array);

        const auto& pkgs1_in_arch = branch1_pkgs.count(arch) ? branch1_pkgs.at(arch) : std::map<std::string, rdbcompare::Package>();
        const auto& pkgs2_in_arch = branch2_pkgs.count(arch) ? branch2_pkgs.at(arch) : std::map<std::string, rdbcompare::Package>();

        // Пакеты только в Ветке 1 и новее в Ветке 1
        for (const auto& pair1 : pkgs1_in_arch) {
            const std::string& pkg_name = pair1.first;
            const rdbcompare::Package& pkg1 = pair1.second;

            if (pkgs2_in_arch.count(pkg_name)) { // Пакет есть в обеих ветках
                const rdbcompare::Package& pkg2 = pkgs2_in_arch.at(pkg_name);
                int cmp_result = rdbcompare::compare_versions(pkg1, pkg2); 
                if (cmp_result > 0) { // pkg1 новее, чем pkg2
                    json_object* diff_entry = json_object_new_object();
                    json_object_object_add(diff_entry, "name", json_object_new_string(pkg_name.c_str()));
                    json_object_object_add(diff_entry, "branch1_version_release", json_object_new_string((pkg1.version + "-" + pkg1.release).c_str()));
                    json_object_object_add(diff_entry, "branch2_version_release", json_object_new_string((pkg2.version + "-" + pkg2.release).c_str()));
                    json_object_array_add(branch1_newer_packages_array, diff_entry); 
                    arch_branch1_newer_count++; 
                }
            } else { //Пакет только в Ветке 1
                json_object_array_add(branch1_only_packages_array, json_object_new_string(pkg_name.c_str())); 
                arch_branch1_only_count++; 
        }

        //Пакеты только в Ветке 2
        for (const auto& pair2 : pkgs2_in_arch) {
            const std::string& pkg_name = pair2.first;
            if (!pkgs1_in_arch.count(pkg_name)) {
                json_object_array_add(branch2_only_packages_array, json_object_new_string(pkg_name.c_str())); 
                arch_branch2_only_count++; 
            }
        }

        
        json_object_object_add(branch1_only_obj, "count", json_object_new_int(arch_branch1_only_count));
        json_object_object_add(branch2_only_obj, "count", json_object_new_int(arch_branch2_only_count));
        json_object_object_add(branch1_newer_obj, "count", json_object_new_int(arch_branch1_newer_count));

        
        json_object_object_add(arch_comparison_json, "branch1_only", branch1_only_obj);
        json_object_object_add(arch_comparison_json, "branch2_only", branch2_only_obj);
        json_object_object_add(arch_comparison_json, "branch1_newer", branch1_newer_obj);

        
        total_branch1_only_count += arch_branch1_only_count;
        total_branch2_only_count += arch_branch2_only_count;
        total_branch1_newer_count += arch_branch1_newer_count;
    }

    // Добавляем раздел "summary" в корневой JSON
    json_object* summary_json = json_object_new_object();
    json_object_object_add(summary_json, "total_branch1_only_count", json_object_new_int(total_branch1_only_count));
    json_object_object_add(summary_json, "total_branch2_only_count", json_object_new_int(total_branch2_only_count));
    json_object_object_add(summary_json, "total_branch1_newer_count", json_object_new_int(total_branch1_newer_count));
    json_object_object_add(result_json, "summary", summary_json);

    const char* json_string = json_object_to_json_string_ext(result_json, JSON_C_TO_STRING_PRETTY);
    char* final_result = strdup(json_string);


    return final_result; // Возвращаем указатель на выделенную память
}

}