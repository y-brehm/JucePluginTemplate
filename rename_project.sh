#!/bin/bash

# This script robustly renames the project, its libraries, all associated
# directories, and all textual occurrences of the old names with context-awareness.

# --- Helper Functions ---
run_sed() {
    # The \b for word boundary is tricky between BSD/macOS and GNU sed.
    # The [<,>] syntax is a portable way to define word boundaries.
    local sed_command="$1"
    if [[ "$OSTYPE" == "darwin"* ]]; then
        # macOS sed
        sed_command=$(echo "$sed_command" | sed -e 's/\\b/[[:<:]]/g' -e 's/\\b/[[:>:]]/g')
    fi

    # Suppress errors for files that can't be modified
    # Note: On macOS, sed -i requires a backup extension. We provide '' for none.
    if [[ "$OSTYPE" == "darwin"* ]]; then
        sed -i '' "$sed_command" "$2" 2>/dev/null
    else
        sed -i "$sed_command" "$2" 2>/dev/null
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
FIRST_CHAR=$(echo "${NEW_NAME:0:1}" | tr '[:upper:]' '[:lower:]')
THE_REST="${NEW_NAME:1}"
NEW_NAME_CAMEL="${FIRST_CHAR}${THE_REST}"

NEW_CORE_FOLDER_NAME="${NEW_NAME_CAMEL}Core"  # e.g., softClipperCore (for paths)
NEW_CORE_TARGET_NAME="${NEW_NAME}Core"      # e.g., SoftClipperCore (for CMake targets)
NEW_NAME_UPPER=$(echo "$NEW_NAME" | tr '[:lower:]' '[:upper:]')

# --- Confirmation ---
echo "------------------------------------"
echo "This will perform the following renames:"
echo "  Project/Class Name:   '$OLD_NAME_PASCAL' -> '$NEW_NAME'"
echo "  Plugin Folder Name:   '$OLD_NAME_CAMEL' -> '$NEW_NAME_CAMEL'"
echo "  Core Folder Name:     '$OLD_CORE_NAME' -> '$NEW_CORE_FOLDER_NAME' (in paths)"
echo "  Core Library Target:  '$OLD_CORE_NAME' -> '$NEW_CORE_TARGET_NAME' (in CMake)"
echo "  Macro Prefix:         '$OLD_NAME_UPPER' -> '$NEW_NAME_UPPER'"
echo "------------------------------------"
read -p "Are you sure? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 1
fi

# --- Execution ---

# **Step 1: Update all file content with precision rules.**
echo "Updating file content..."

# Find all files to process, excluding git, build, cache, and third-party dirs,
# as well as this script itself and the project README.
FILES_TO_PROCESS=$(find . -type f \
    -not -path "./.git/*" \
    -not -path "*/build/*" \
    -not -path "*/.cache/*" \
    -not -path "*/ThirdParty/*" \
    -not -name "$(basename "$0")" \
    -not -name "README.md")

for file in $FILES_TO_PROCESS; do
    # CRITICAL ORDER: The most specific replacements must run first.

    # 1. Replace include paths first. This requires the camelCase FOLDER name.
    #    This handles both #include "..." and #include <...>
    run_sed "s|\([\"<]\)${OLD_CORE_NAME}/|\1${NEW_CORE_FOLDER_NAME}/|g" "$file"

    # 2. Replace the standalone core name token (for CMake targets). This uses
    #    the PascalCase TARGET name and word boundaries (\b) to avoid replacing
    #    parts of other words or paths.
    run_sed "s/\b${OLD_CORE_NAME}\b/${NEW_CORE_TARGET_NAME}/g" "$file"

    # 3. Run the rest of the standard replacements.
    run_sed "s/${OLD_NAME_PASCAL}/${NEW_NAME}/g" "$file"
    run_sed "s/${OLD_NAME_CAMEL}/${NEW_NAME_CAMEL}/g" "$file"
    run_sed "s/${OLD_NAME_UPPER}/${NEW_NAME_UPPER}/g" "$file"
done

# **Step 2: Rename directories.**
echo "Renaming directories..."
TOP_LEVEL_PLUGIN_DIR="$OLD_NAME_CAMEL"
NESTED_CORE_DIR="${TOP_LEVEL_PLUGIN_DIR}/${OLD_CORE_NAME}"
DEEPEST_INCLUDE_DIR="${NESTED_CORE_DIR}/include/${OLD_CORE_NAME}"

if [ -d "${DEEPEST_INCLUDE_DIR}" ]; then mv "${DEEPEST_INCLUDE_DIR}" "${NESTED_CORE_DIR}/include/${NEW_CORE_FOLDER_NAME}"; echo "  Renamed: ${DEEPEST_INCLUDE_DIR}"; fi
if [ -d "${NESTED_CORE_DIR}" ]; then mv "${NESTED_CORE_DIR}" "${TOP_LEVEL_PLUGIN_DIR}/${NEW_CORE_FOLDER_NAME}"; echo "  Renamed: ${NESTED_CORE_DIR}"; fi
if [ -d "${TOP_LEVEL_PLUGIN_DIR}" ]; then mv "${TOP_LEVEL_PLUGIN_DIR}" "$NEW_NAME_CAMEL"; echo "  Renamed: ${TOP_LEVEL_PLUGIN_DIR}"; fi

echo "✅ Renaming complete!"
echo "Please verify changes before committing."
