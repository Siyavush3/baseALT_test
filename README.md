# **ALT Linux RDB Package Comparison Tool**

This project provides a shared C++ library and a Python command-line interface (CLI) utility to compare binary package lists from different branches of the ALT Linux repository database (RDB).

## **The Original Task**

This project was developed as a test assignment with the following requirements:

We have a public REST API for our database: https://rdb.altlinux.org/api/.  
It provides an endpoint: /export/branch\_binary\_packages/{branch} (e.g., sisyphus, p10, p9).  
The goal was to create a shared library (following [TLDP's Program Library HOWTO](https://tldp.org/HOWTO/Program-Library-HOWTO/shared-libraries.html)) and a CLI utility that:

1. **Fetches binary package lists** for two specified branches (e.g., sisyphus and p10).  
2. **Compares the retrieved package lists** and outputs a JSON structure (whose format was to be designed) showing:  
   * All packages present in the 1st branch but not in the 2nd.  
   * All packages present in the 2nd branch but not in the 1st.  
   * All packages whose version-release is newer in the 1st branch compared to the 2nd.  
3. This comparison must be performed for **each supported architecture** (based on the arch field in the API response).

The development process was to be recorded in a Git repository with an unrewritten history. The utility must run on Linux (specifically, it will be tested on ALT Workstation K 10.1) and include a README with installation and usage instructions following the FHS standard. Any necessary libraries must be sourced from the ALT Linux repository (https://packages.altlinux.org/ru/p10/).

## **Features**

Our CLI utility rdb\_compare offers the following capabilities:

* **Fetches package lists** from https://rdb.altlinux.org/api/export/branch\_binary\_packages/{branch}.  
* **Compares package sets** across architectures.  
* **Identifies package differences**:  
  * Packages unique to Branch 1\.  
  * Packages unique to Branch 2\.  
  * Packages with a newer version-release in Branch 1\.  
* **Flexible output formats**:  
  * Human-readable, hierarchical **tree output** (default).  
  * Raw **JSON output** directly from the C++ library.  
  * Raw JSON list for a **single branch**.  
* **Command-line options** for specifying branches, filtering comparison categories, and managing output format.  
* Includes \--help and \--version options.

## **Prerequisites**

To build and run this project, you'll need the following on an ALT Linux system (or a compatible RPM-based distribution):

* **C++ Compiler:** g++ (version 13 or newer recommended, as used in development).  
* **Make Utility:** make.  
* **Development Libraries:**  
  * libcurl-devel: For making HTTP requests.  
  * libjson-c-devel: For JSON parsing in C++.  
  * librpm-devel: Provides RPM version comparison utilities (rpmvercmp).  
* **Python 3:** (version 3.10+ recommended).  
* **Python pip:** Python package installer.

### **Installation of Prerequisites on ALT Linux**

Open your terminal and run:
```
sudo apt-get update  
sudo apt-get install git g++ make libcurl-devel libjson-c-devel librpm-devel python3 python3-module-pip
```
## **Installation**

Follow these steps to build and install the library and the CLI tool:

1. **Clone the Repository:**  
```
   git clone https://github.com/Siyavush3/baseALT\_test.git  
   cd baseALT_test
   ```

2. Build the C++ Shared Library:  
   This command compiles the C++ source code and creates librdbcompare.so in the build/lib/ directory. Object files will be in build/obj/.  
   ```
   make
   ```

3. Install the C++ Shared Library:  
   This step copies librdbcompare.so to /usr/lib/ and rdbcompare.hpp to /usr/include/. It also updates the system's dynamic linker cache.  
   ```
   sudo make install
   ```
   *You might need to run sudo ldconfig manually if the utility still reports "library not found" after installation.*  
4. Install the Python CLI Utility:  
   This command copies rdb\_compare\_cli.py to /usr/bin/ and makes it executable.  
   ```
   sudo make install_cli
   ```

## **Usage**

Once installed, you can run the rdb\_compare utility from any directory.
```
rdb_compare [BRANCH1] [BRANCH2] [OPTIONS]
```

* BRANCH1: Name of the first branch (default: sisyphus).  
* BRANCH2: Name of the second branch (default: p10 for comparison).

### **Examples:**

1. **Compare sisyphus and p10 (default branches), showing all differences in human-readable format:**  
   ```
   rdb_compare  
   ```
   \# or explicitly:  
   ```
   rdb_compare sisyphus p10
   ```

2. **Compare p9 and p10, showing only packages unique to p9:**  
   ```
   rdb_compare p9 p10 --category branch1_only
   ```

3. **Compare sisyphus and p10, showing only packages newer in sisyphus:**  
   ```
   rdb_compare sisyphus p10 --category branch1_newer
   ```

4. Get the raw JSON package list for a single branch (p9):  
   This ignores BRANCH2 and comparison options.  
   ```
   rdb_compare --show-branch-json p9
   ```

5. **Get the raw JSON comparison result (unformatted by Python):**  
   ```
   rdb_compare sisyphus p10 --json
   ```

6. **Display the utility's version:**
   ```
   rdb_compare --version
   ```

7. **Show help message:**
   ```  
   rdb_compare --help
   ```
## **Project Structure**

The project follows a standard layout to separate source code, build artifacts, and installation targets:
   ```
project\_root/  
+-- src/                 \# Source files  
¦   +-- lib/             \# C++ library source (rdbcompare.cpp, rdbcompare.hpp)  
¦   +-- cli/             \# Python CLI source (rdb\_compare\_cli.py)  
+-- build/               \# Compiled artifacts (obj, lib)  
+-- include/             \# Public headers for system installation  
+-- Makefile             \# Build automation  
+-- README.md            \# This file  
+-- ...                  \# Other optional directories (tests, doc)
   ```
## **Development Notes**

* **C++ Shared Library (librdbcompare.so):** Implemented using libcurl for HTTP requests, json-c for JSON parsing, and librpm's rpmvercmp for version comparison.  
* **Python CLI (rdb\_compare\_cli.py):** Utilizes ctypes to interface with the C++ library, argparse for command-line argument parsing, and json for handling JSON data.  
* **Memory Management:** The C++ library allocates strings using strdup(). The Python CLI explicitly frees this memory using libc.free() to prevent memory leaks. This interaction is carefully handled by ctypes.POINTER(ctypes.c\_char) and ctypes.string\_at().  
* **RPM Version Comparison:** Due to the absence of rpmevrcmp on some systems, the compare\_versions function uses a layered approach with rpmvercmp to compare epoch, then version, then release. While highly accurate, it may not cover every single edge case of rpmevrcmp.