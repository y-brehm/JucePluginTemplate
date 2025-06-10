# PluginTemplate

A modern C++ template for creating audio plugins with a Web UI using the **JUCE** framework and a **CMake**-based build system.

This template is designed to be a robust starting point for new projects, featuring a clean architecture where the core DSP is separated from the main plugin logic.

## Key Features

  * **Modern CMake Build System**: Simple, fast, and IDE-agnostic.
  * **Web-Based UI**: Uses JUCE's `WebBrowserComponent` to allow UI development with standard HTML, CSS, and JavaScript.
  * **Separated DSP Core**: The audio processing logic (`pluginTemplateCore`) is built as a separate static library, promoting a clean, modular, and testable design.
  * **Automated Renaming Script**: A shell script (`rename_project.sh`) is included to automate the process of renaming the project, directories, and library targets.
  * **Cross-Platform**: Ready for development on both macOS and Windows.

## Getting Started

Follow these steps to set up your development environment and create your first project from this template.

### Step 1: Create Your Repository

Click the green **"Use this template"** button on the GitHub repository page to create your own new repository based on this template. You will start with a clean Git history.

### Step 2: Prerequisites & Setup

Install the necessary build tools for your operating system using a package manager.

#### macOS

The tools were tested with: `Apple Clang 14`, `CMake 3.30`, `Ninja 1.12`, `Git 2.39`.

1.  **Install Xcode Command Line Tools**: If you haven't already, open the Terminal and run:
    ```bash
    xcode-select --install
    ```
2.  **Install Xcode from the macOS App-Store or use ninja as a sleek generator**:
    The App-Store version of ninja is not necessarily needed in case you use ninja as a generator.
    There might occur some issues if you want to create AU-plugins with ninja though.

3.  **Install Tools with a packet manager like Homebrew**:
    Brew is a terminal installer for macOS and Linux. Download and install from [brew.sh](https://brew.sh/).

    In case you decided to get Xcode from the App-Store you can skip ninja.
    ```bash
    brew install cmake ninja
    ```

#### Windows

1.  **Install C++ Compiler**: Install **Visual Studio 2022** with the **"Desktop development with C++"** workload. This provides the required MSVC compiler toolchain. Download and install from [visualstudio.microsoft.com](https://visualstudio.microsoft.com/).

2.  **Install Git for Windows**: Download and install from [git-scm.com](https://git-scm.com/download/win). This also provides **Git Bash**, which is required to run the renaming script.

3.  **Install Tools with Scoop**: Scoop is a command-line installer for Windows. Download and install from [scoop.sh](https://scoop.sh/).

      * Use scoop to, install CMake, nuget (needed to install the Windows Web View package in a future CMake step) and optionally Ninja (sleek CMake generator):
        ```powershell
        scoop install cmake ninja
        ```

### Step 3: Clone Your New Repository

In your terminal or Git Bash, clone the repository you just created in Step 1.

```bash
git clone https://github.com/YourUsername/YourNewRepository.git
cd YourNewRepository
```

### Step 4: Customize Your Plugin

Run the included shell script to rename the project files and targets.

> **Note for Windows users**: You must run this command from **Git Bash**, not Command Prompt or PowerShell.

```bash
./rename_project.sh
```

The script will prompt you for a new project name (e.g., `MyStonksFilter`) and automatically handle renaming the project, directories, and CMake targets.

## Building the Project

This project uses CMake and Ninja for fast builds.

1.  **Configure the project**: Run CMake to generate the build files. This command creates a `build` directory.
    ```bash
    # macOS
    cmake -G "Xcode" -B build 

    # Windows
    cmake -G "Visual Studio 17 2022" -B build

    # in case you installed Ninja
    cmake -G Ninja -B build 
    ```
2.  **Run the build**:
    ```bash
    cmake --build build
    ```
3.  **Find your plugins**: The compiled plugin files (`.vst3`, `.component`) will be located in the `build/PluginTemplate_artefacts/` directory.

## Project Structure

  * `CMakeLists.txt`: The root CMake file.
  * `pluginTemplate/`: The main plugin source folder. Contains the `PluginProcessor` and `PluginEditor`.
  * `pluginTemplateCore/`: The separated, self-contained DSP library.
  * `WebUI/`: Contains the HTML, CSS, and JS for the plugin's user interface.
  * `rename_project.sh`: The script to automate project renaming.

## Licensing

This template is dual-licensed under the **MIT** and **GPLv3** licenses. You can choose which one to use.

  * **MIT License**: If you have a commercial JUCE license and wish to create a closed-source product, you may use this template under the MIT license.
  * **GPLv3 License**: If you are using the open-source version of JUCE (governed by the AGPLv3), you probably must use this template under the terms of the GPLv3. This means your resulting plugin must also be open-source and GPLv3-compatible.
