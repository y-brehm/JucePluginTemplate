#!/bin/bash

# This script renames the project, the core library, and all associated directories.

run_sed() {
  if [[ "$OSTYPE" == "darwin"* ]]; then
    sed -i '' "$1" "$2"
  else
    sed -i "$1" "$2"
  fi
}

echo "--- PluginTemplate Project Renamer ---"

read -p "Enter new Project Name in PascalCase (e.g., MyCoolFilter): " NEW_NAME

if [ -z "$NEW_NAME" ]; then
    echo "Error: No name entered. Exiting."
    exit 1
fi

FIRST_CHAR=$(echo "${NEW_NAME:0:1}" | tr '[:upper:]' '[:lower:]')
THE_REST="${NEW_NAME:1}"
NEW_FOLDER_NAME="${FIRST_CHAR}${THE_REST}"

NEW_CORE_FOLDER_NAME="${NEW_FOLDER_NAME}Core"
NEW_CORE_TARGET_NAME="${NEW_NAME}Core"


echo "------------------------------------"
echo "This will perform the following renames:"
echo "  Project Name:     'PluginTemplate' -> '$NEW_NAME'"
echo "  Plugin Folder:    'pluginTemplate' -> '$NEW_FOLDER_NAME'"
echo "  Core Folder:      'pluginTemplateCore' -> '$NEW_CORE_FOLDER_NAME'"
echo "  Core Library:     'pluginTemplateCore' -> '$NEW_CORE_TARGET_NAME'"
read -p "Are you sure? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]
then
    exit 1
fi

# --- Perform Renames ---
echo "Updating CMake files and source code..."
run_sed "s/project(PluginTemplate/project(${NEW_NAME}/g" CMakeLists.txt
run_sed "s/add_subdirectory(pluginTemplateCore/add_subdirectory(${NEW_CORE_FOLDER_NAME}/g" CMakeLists.txt
run_sed "s/pluginTemplateCore/${NEW_CORE_TARGET_NAME}/g" pluginTemplateCore/CMakeLists.txt
run_sed "s/pluginTemplateCore/${NEW_CORE_TARGET_NAME}/g" pluginTemplate/CMakeLists.txt
run_sed "s|#include <pluginTemplateCore/|#include <${NEW_CORE_FOLDER_NAME}/|g" include/PluginProcessor.h

echo "Renaming directories..."
mv pluginTemplateCore/include/pluginTemplateCore "pluginTemplateCore/include/${NEW_CORE_FOLDER_NAME}"
mv pluginTemplateCore "$NEW_CORE_FOLDER_NAME"
mv pluginTemplate "$NEW_FOLDER_NAME"


echo "✅ Renaming complete!"
echo "Please manually edit the metadata (manufacturer, plugin codes) in ${NEW_FOLDER_NAME}/CMakeLists.txt."
