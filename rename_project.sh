#!/bin/bash

# This script robustly renames the project, its libraries, all associated
# directories, and all textual occurrences of the old names.

run_sed() {
    if [[ "$OSTYPE" == "darwin"* ]]; then
        sed -i '' "$1" "$2" 2>/dev/null
    else
        sed -i "$1" "$2" 2>/dev/null
    fi
}

# --- Configuration ---
OLD_NAME_PASCAL="PluginTemplate"
OLD_NAME_CAMEL="pluginTemplate"
OLD_NAME_UPPER="PLUGINTEMPLATE"
OLD_CORE_NAME="pluginTemplateCore"

# --- User Input ---
echo "--- PluginTemplate Project Renamer ---"
read -p "Enter new Project Name in PascalCase (e.g., MyCoolFilter): " NEW_NAME

if [ -z "$NEW_NAME" ]; then
    echo "Error: No name entered. Exiting."
    exit 1
fi

# --- Generate New Names ---
# Convert PascalCase to camelCase (e.g., MyCoolFilter -> myCoolFilter)
FIRST_CHAR=$(echo "${NEW_NAME:0:1}" | tr '[:upper:]' '[:lower:]')
THE_REST="${NEW_NAME:1}"
NEW_NAME_CAMEL="${FIRST_CHAR}${THE_REST}"

# Generate the Core library/folder names
NEW_CORE_FOLDER_NAME="${NEW_NAME_CAMEL}Core"  # e.g., myCoolFilterCore
NEW_CORE_TARGET_NAME="${NEW_NAME}Core"      # e.g., MyCoolFilterCore

# Generate an all-uppercase version for preprocessor macros
NEW_NAME_UPPER=$(echo "$NEW_NAME" | tr '[:lower:]' '[:upper:]')

# --- Confirmation ---
echo "------------------------------------"
echo "This will perform the following renames:"
echo "  Project/Class Name:   '$OLD_NAME_PASCAL' -> '$NEW_NAME'"
echo "  Plugin Folder Name:   '$OLD_NAME_CAMEL' -> '$NEW_NAME_CAMEL'"
echo "  Core Folder Name:     '$OLD_CORE_NAME' -> '$NEW_CORE_FOLDER_NAME'"
echo "  Core Library Target:  '$OLD_CORE_NAME' -> '$NEW_CORE_TARGET_NAME'"
echo "  Macro Prefix:         '$OLD_NAME_UPPER' -> '$NEW_NAME_UPPER'"
echo "------------------------------------"
read -p "Are you sure? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 1
fi

# --- Execution ---

# **Step 1: Update all file content.**
echo "Updating file content (skipping .git, build, and ThirdParty)..."
FILES=$(find . -type f -not -path "./.git/*" -not -path "*/build/*" -not -path "*/ThirdParty/*" -not -name "$(basename "$0")")

for file in $FILES; do
    run_sed "s|#include <${OLD_CORE_NAME}/|#include <${NEW_CORE_FOLDER_NAME}/|g" "$file"
    run_sed "s/${OLD_CORE_NAME}/${NEW_CORE_TARGET_NAME}/g" "$file"
    run_sed "s/${OLD_NAME_PASCAL}/${NEW_NAME}/g" "$file"
    run_sed "s/${OLD_NAME_CAMEL}/${NEW_NAME_CAMEL}/g" "$file"
    run_sed "s/${OLD_NAME_UPPER}/${NEW_NAME_UPPER}/g" "$file"
done

# **Step 2: Rename directories.**
echo "Renaming directories..."

TOP_LEVEL_PLUGIN_DIR="$OLD_NAME_CAMEL"
NESTED_CORE_DIR="${TOP_LEVEL_PLUGIN_DIR}/${OLD_CORE_NAME}"
DEEPEST_INCLUDE_DIR="${NESTED_CORE_DIR}/include/${OLD_CORE_NAME}"

# 1. Rename the deepest directory first: pluginTemplate/pluginTemplateCore/include/pluginTemplateCore
if [ -d "${DEEPEST_INCLUDE_DIR}" ]; then
    mv "${DEEPEST_INCLUDE_DIR}" "${NESTED_CORE_DIR}/include/${NEW_CORE_FOLDER_NAME}"
    echo "  Renamed: ${DEEPEST_INCLUDE_DIR} -> ${NESTED_CORE_DIR}/include/${NEW_CORE_FOLDER_NAME}"
else
    echo "  Info: Deepest include directory not found at '${DEEPEST_INCLUDE_DIR}', skipping."
fi

# 2. Rename the nested core directory: pluginTemplate/pluginTemplateCore
if [ -d "${NESTED_CORE_DIR}" ]; then
    mv "${NESTED_CORE_DIR}" "${TOP_LEVEL_PLUGIN_DIR}/${NEW_CORE_FOLDER_NAME}"
    echo "  Renamed: ${NESTED_CORE_DIR} -> ${TOP_LEVEL_PLUGIN_DIR}/${NEW_CORE_FOLDER_NAME}"
else
    echo "  Info: Nested core directory not found at '${NESTED_CORE_DIR}', skipping."
fi

# 3. Rename the top-level plugin directory: pluginTemplate
if [ -d "${TOP_LEVEL_PLUGIN_DIR}" ]; then
    mv "${TOP_LEVEL_PLUGIN_DIR}" "$NEW_NAME_CAMEL"
    echo "  Renamed: ${TOP_LEVEL_PLUGIN_DIR} -> ${NEW_NAME_CAMEL}"
else
    echo "  Warning: Top-level plugin directory '${TOP_LEVEL_PLUGIN_DIR}' not found. Cannot complete rename."
fi


echo "✅ Renaming complete!"
echo "Please check git status for a full list of changes."
echo "Remember to manually edit metadata (manufacturer, plugin codes) if they are not derived from the project name."
