#ifndef RDBCOMPARE_HPP
#define RDBCOMPARE_HPP

#ifdef __cplusplus
extern "C" {
#endif

void rdbcompare_init();
void rdbcompare_cleanup();

char* fetch_package_list(const char* branch);


char* compare_packages(const char* branch1_data, const char* branch2_data);

#ifdef __cplusplus
}
#endif

#endif // RDBCOMPARE_HPP