# **ALT Linux RDB Package Comparison Tool**

This project provides a shared C++ library and a Python command-line interface (CLI) utility, along with a **Qt-based Graphical User Interface (GUI) application**, to compare binary package lists from different branches of the ALT Linux repository database (RDB).

## **The Original Task (Updated)**

This project was developed as a test assignment with the following requirements:

We have a public REST API for our database: https://rdb.altlinux.org/api/.  
It provides an endpoint: /export/branch\_binary\_packages/{branch} (e.g., sisyphus, p10, p9).  
The goal was to create a utility with the following components:

### **GUI Interface (Qt)**

1. **Branch Selection**: Allow selection of two branches for comparison (e.g., sisyphus and p11).  
2. **Tabular Results Display**: Display comparison results in a table, showing:  
   * The **count** of packages present in the 1st branch but not in the 2nd.  
   * The **count** of packages present in the 2nd branch but not in the 1st.  
   * The **count** of packages whose version-release is newer in the 1st branch compared to the 2nd.  
   * Results must be separated by supported architectures (based on the arch field in the API response).  
3. **JSON Export**: Allow saving information about packages that fall into different categories as a JSON file in the user's home directory.

### **Library (C++ Shared Library)**

1. **Shared Library Compliance**: Must be built according to [TLDP's Program Library HOWTO recommendations](https://tldp.org/HOWTO/Program-Library-HOWTO/shared-libraries.html).  
2. **Package List Retrieval**: Must retrieve binary package lists for two specified branches.  
3. **Comparison Logic**: Must compare the retrieved package lists and **count** packages based on the conditions defined above for the GUI.

### **General Development Requirements**

1. **Git Repository**: The development process must be documented in a Git repository with an unrewritten history and hosted (e.g., on GitHub).  
2. **ALT Linux Repository Libraries**: Any necessary libraries must be sourced from the ALT Linux repository (https://packages.altlinux.org/ru/p11/).  
3. **Linux Compatibility**: The utility must run on the Linux operating system (specifically, it will be tested on ALT Workstation K 11.1).  
4. **README**: A README in English must be provided with installation and usage instructions following the FHS standard.

## **Features**

Our rdb\_compare tool offers comprehensive functionality:

* **Core C++ Library (librdbcompare.so)**:  
  * Fetches package lists from https://rdb.altlinux.org/api/.  
  * Compares package sets across architectures, providing **counts** and detailed lists of differences (packages unique to Branch 1, unique to Branch 2, or newer in Branch 1).  
  * Utilizes libcurl for HTTP requests, json-c for JSON parsing, and librpm's rpmvercmp for version comparison.  
  * Adheres to shared library best practices (SONAME, proper initialization/cleanup).  
* **Python CLI Utility (rdb\_compare\_cli.py)**:  
  * A command-line interface for direct interaction with the C++ library.  
  * Provides flexible output formats (human-readable tree, raw JSON, single branch JSON).  
  * Includes \--help and \--version options.  
* **Qt GUI Application (alt\_rdb\_gui\_app)**:  
  * Intuitive graphical interface for selecting branches.  
  * Displays comparison results in a sortable and filterable table (by package name and architecture).  
  * Shows summary counts for each difference category.  
  * Allows saving filtered table data to a JSON file in the user's home directory.  
  * Performs operations asynchronously to keep the UI responsive.  
  * Includes an "Cancel" button to interrupt ongoing comparisons.  
  * Provides clear error and status messages in the GUI.

## **Prerequisites**

To build and run this project, you'll need the following on an ALT Linux system (or a compatible RPM-based distribution):

* **C++ Compiler:** g++ (version 13 or newer recommended).  
* **Make Utility:** make.  
* **Git:** For cloning the repository.  
* **Essential Build Tools:** build-essential.  
* **Development Libraries:**  
  * libcurl-devel: For making HTTP requests.  
  * libjson-c-devel: For JSON parsing in C++.  
  * librpm-devel: Provides RPM version comparison utilities (rpmvercmp).  
* **Python 3:** (version 3.10+ recommended).  
* **Python pip:** Python package installer (python3-module-pip).  
* **Qt 5 Development Libraries:**  
  * qt5-base-devel: Core Qt 5 libraries.  
  * dqt5-tools-devel: Qt 5 development tools (including qmake, Qt Designer).  
* **Utility for Line Endings:** dos2unix.

### **Installation of Prerequisites on ALT Linux**

Open your terminal and run the following command as root:
```
sudo apt-get update \# Update package lists first

sudo apt-get install \\  
    git \\  
    g++ \\  
    make \\  
    build-essential \\  
    libcurl-devel \\  
    libjson-c-devel \\  
    librpm-devel \\  
    python3 \\  
    python3-module-pip \\  
    qt5-base-devel \\  
    dqt5-tools-devel \\  
    dos2unix
```

## **Installation**

Follow these steps to build and install the shared library, the CLI tool, and the GUI application:

1. **Clone the Repository:**  
```
   git clone https://github.com/Siyavush3/baseALT\_test.git  
   cd baseALT\_test
```
2. Build the C++ Shared Library:  
   This command compiles the C++ source code (src/lib/rdbcompare.cpp) and creates librdbcompare.so.1.0.0 in the build/lib/ directory. Object files will be in build/obj/.  
   ```
   make
   ```
3. Install the C++ Shared Library:  
   This step copies librdbcompare.so.1.0.0 to /usr/lib/, creates necessary symbolic links (librdbcompare.so.1, librdbcompare.so), and copies rdbcompare.hpp to /usr/include/. It also updates the system's dynamic linker cache.  
   ```
   sudo make install
   ```
   *You might need to run sudo ldconfig manually if the utility still reports "library not found" after installation, although make install attempts to do this.*  
4. Install the Python CLI Utility:  
   This command copies rdb\_compare\_cli.py from src/cli/ to /usr/bin/rdb\_compare and makes it executable.  
   ```
   sudo make install_cli
   ```
5. **Prepare for Qt GUI Application Compilation:**  
   * **Navigate to the GUI project directory:** 
   ``` 
     cd src/gui
   ```
   * **Ensure qmake is in your PATH:** qmake is part of dqt5-tools-devel. You might need to add its directory (e.g., /usr/share/dqt5/bin/) to your PATH environment variable if it's not found automatically.  
   ```
     export PATH=$PATH:/usr/share/dqt5/bin \# Add this if 'qmake \--version' fails  
     qmake \--version \# Verify qmake is now found
   ```

   * Generate the Qt Project Makefile:  
     This command reads alt\_rdb\_gui\_app.pro and creates a Makefile specific to the Qt application in the current directory.  
     ```
     qmake
     ```

6. Build the Qt GUI Application:  
   This compiles main.cpp, mainwindow.cpp, comparisonworker.cpp and links them with Qt libraries and librdbcompare.so, creating the alt\_rdb\_gui\_app executable.  
   ```
   make
   ```

## **Usage**

### **1\. Using the Qt GUI Application**

Once built, you can run the alt\_rdb\_gui\_app from its build directory (src/gui/).
```
cd /path/to/your/project/src/gui  
./alt\_rdb\_gui\_app
```

**GUI Interface Details:**

* **Top Section**: Application title, two input fields for Branch 1 and Branch 2 names (with defaults like sisyphus and p10), and a "Start Comparison" button.  
* **Middle Section**:  
  * Summary counts: Text labels displaying "Only in Branch 1: X", "Only in Branch 2: Y", "Newer in Branch 1: Z".  
  * Filters: A text input field for filtering by package name, and a dropdown (QComboBox) for filtering by architecture.  
  * Results Table: A QTableWidget with columns for "Architecture", "Package Name", "Epoch", "Version (B1)", "Release (B1)", "Version (B2)", "Release (B2)", and "Category". The table supports sorting by columns.  
* **Bottom Section**: An errorLabel for displaying status and error messages, and two buttons in the bottom-right corner: "Cancel" (to interrupt an ongoing comparison) and "Save to JSON" (to save the currently filtered table data).

### **2\. Using the Python CLI Utility**

Once installed, you can run the rdb\_compare utility from any directory.
```
rdb_compare [BRANCH1] [BRANCH2] [OPTIONS]
```

* BRANCH1: Name of the first branch (default: sisyphus).  
* BRANCH2: Name of the second branch (default: p10 for comparison).

**Examples:**

1. **Compare sisyphus and p10 (default branches), showing all differences in human-readable format:**  
```
   rdb_compare  
   # or explicitly:  
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
├── src/                 \# Source files  
│   ├── lib/             \# C++ library source (rdbcompare.cpp, rdbcompare.hpp)  
│   ├── cli/             \# Python CLI source (rdb\_compare\_cli.py)  
│   └── gui/             \# Qt GUI application source (alt\_rdb\_gui\_app.pro, \*.cpp, \*.h)  
├── build/               \# Compiled artifacts (obj, lib)  
├── include/             \# Public headers for system installation  
├── Makefile             \# Build automation  
├── README.md            \# This file  
└── ...                  \# Other optional directories (tests, doc)
```
## **Development Notes**

* **C++ Shared Library (librdbcompare.so):** Implemented using libcurl for HTTP requests, json-c for JSON parsing, and librpm's rpmvercmp for version comparison.  
* **Python CLI (rdb\_compare\_cli.py):** Utilizes ctypes to interface with the C++ library, argparse for command-line argument parsing, and json for handling JSON data.  
* **Qt GUI Application (alt\_rdb\_gui\_app):** Developed using Qt Widgets. It interacts with the C++ library asynchronously using QThread to ensure UI responsiveness.  
* **Memory Management:** The C++ library allocates strings using strdup(). Both the Python CLI and the Qt GUI explicitly free this memory using libc.free() (in Python) or free() (in C++) to prevent memory leaks. This interaction is carefully handled by ctypes.POINTER(ctypes.c\_char) and ctypes.string\_at() in Python, and direct C++ free() in Qt.  
* **RPM Version Comparison:** Due to the absence of rpmevrcmp on some systems, the compare\_versions function uses a layered approach with rpmvercmp to compare epoch, then version, then release. While highly accurate, it may not cover every single edge case of rpmevrcmp.  
* **Qt Global Initialization:** curl\_global\_init() and curl\_global\_cleanup() are called once in the main() function of the Qt application's main thread to ensure proper libcurl initialization and cleanup for multi-threaded usage, as per libcurl's documentation.
