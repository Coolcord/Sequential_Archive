#!/bin/bash

# This must be set to the parent directory of Sequential_Archive!
localSourceCodeLocation="/d/Documents/Source_Code"

# Make sure a version number was provided
if [ -z $1 ]; then
    echo "No version argument provided! The first argument must be the version!"
    echo ""
    echo "$0 <version>"
    echo ""
    echo "E.g.: $0 v0.4.0"
    exit 1
fi

# Remove old deployed files
rm -rf ./Deployed_Files >/dev/null 2>&1

# Get dependencies for Deploy Script
if [ ${MSYSTEM} == "MINGW64" ]; then
    dependencies="git rsync mingw-w64-x86_64-gcc mingw-w64-x86_64-ninja mingw-w64-x86_64-cmake mingw-w64-x86_64-qt6-base p7zip mingw-w64-x86_64-nsis"
    echo Checking dependencies for deploy...
    if ! pacman -Q $dependencies > /dev/null 2>&1; then
        echo Installing missing dependencies...
        pacman -Sy --needed --noconfirm $dependencies
    fi
fi

# Check if dependencies are installed
if [ ! -z $1 ] && ([ "$1" == "latest" ] || [ "$1" == "local" ]); then # TODO: After the next update, this code will be applied to stable!
    echo ""; echo [1/11] Preparing source code...
    command -v ninja >/dev/null 2>&1 || { echo >&2 "ninja must be installed before Sequential_Archive can be compiled! Aborting!"; exit 1; }
fi
command -v git >/dev/null 2>&1 || { echo >&2 "git must be installed before Sequential_Archive can be compiled! Aborting!"; exit 1; }
command -v gcc >/dev/null 2>&1 || { echo >&2 "gcc must be installed before Sequential_Archive can be compiled! Aborting!"; exit 1; }
command -v nproc >/dev/null 2>&1 || { echo >&2 "nproc must be installed before Sequential_Archive can be compiled! Aborting!"; exit 1; }
if [ ${MSYSTEM} == "MINGW64" ]; then
    command -v qtpaths6 >/dev/null 2>&1 || { echo >&2 "qtpaths6 must be installed before Sequential_Archive can be compiled! Aborting!"; exit 1; }
    command -v ldd >/dev/null 2>&1 || { echo >&2 "ldd must be installed before Sequential_Archive can be compiled! Aborting!"; exit 1; }
    if [ ! -f /mingw64/share/qt6/plugins/platforms/qwindows.dll ]; then
        echo "qwindows.dll could not be found! Aborting!"; exit 1;
    fi
    if [ ! -f /mingw64/share/qt6/plugins/styles/qwindowsvistastyle.dll ]; then
        echo "qwindowsvistastyle.dll could not be found! Aborting!"; exit 1;
    fi
	if [ ! -f /mingw64/share/qt6/plugins/tls/qschannelbackend.dll ]; then
        echo "qschannelbackend.dll could not be found! Aborting!"; exit 1;
    fi
fi
command -v 7z >/dev/null 2>&1 || { echo >&2 "p7zip must be installed before Sequential_Archive can be deployed! Aborting!"; exit 1; }
command -v makensis >/dev/null 2>&1 || { echo >&2 "NSIS must be installed before Sequential_Archive can be deployed! Aborting!"; exit 1; }
CPUcores=$(nproc)

# Add v to the version if it doesn't exist
version="$1"
if [[ "${original}" != v* ]]; then
    modified="v${original}"
fi
versionNumber="${version:1}"

# Split the parts into significant, major, minor, and patch versions
IFS=. read -r significant major minor <<< "$versionNumber"
IFS=- read -r minor patch <<< "$minor"
if [ -z $patch ]; then
    patch="0"
fi
if [[ $version == *-dev ]]; then
    patch="0"
fi
installerVersion="$significant.$major.$minor.$patch"

# Update the Sequential_Archive version
sed -i "s/    const static QString VERSION_NUMBER = .*/    const static QString VERSION_NUMBER = \"$versionNumber\"\;/g" "$localSourceCodeLocation"/Sequential_Archive/Sequential_Archive_Manager/Version.h
cd "$localSourceCodeLocation"/Sequential_Archive

# Build Sequential_Archive
echo ""; echo [1/3] Compiling Sequential Archive...
cd Sequential_Archive/
rm -rf build >/dev/null 2>&1; mkdir -p build
cd build
cmake .. -G Ninja
ninja -j "$CPUcores" || exit 1
cd ../../

echo ""; echo [2/3] Compiling SAM...
cd Sequential_Archive_Manager/
rm -rf build >/dev/null 2>&1; mkdir -p build
cd build
cmake .. -G Ninja
ninja -j "$CPUcores" || exit 1
cd ../../

# Create the Runtime Environment
mkdir -p "$localSourceCodeLocation"/Sequential_Archive/Sequential_Archive_Manager/Build_Scripts/Unix/Deployed_Files
cd "$localSourceCodeLocation"/Sequential_Archive/Sequential_Archive_Manager/Build_Scripts/Unix/Deployed_Files
mkdir -p ./SAM/Plugins

# Install Plugins
echo ""; echo [3/3] Installing Plugins...
if [ -z ${MSYSTEM} ]; then # assume we're on GNU/Linux or Mac
    dllExt=".so"
    exeExt=""
else # assume we're on a Unix environment running in a Windows OS
    dllExt=".dll"
    exeExt=".exe"
fi

cp "$localSourceCodeLocation"/Sequential_Archive/Sequential_Archive_Manager/build/Sequential_Archive_Manager"$exeExt" ./SAM/SAM"$exeExt"
chmod +x ./SAM/SAM"$exeExt"
cp "$localSourceCodeLocation"/Sequential_Archive/Sequential_Archive/build/libSequential_Archive"$dllExt" ./SAM/Plugins/Sequential_Archive"$dllExt"
cp -f "$localSourceCodeLocation/Sequential_Archive/Sequential_Archive_Manager/Build_Scripts/Windows/Installer/UPDATES AND SOURCE CODE.TXT" ./SAM

# Install Qt DLLs
if [ ${dllExt} == ".dll" ]; then
    qtpaths6Location=$(which qtpaths6.exe)
    qtDLLsLocation=${qtpaths6Location%/*}
    
    # Install Qt Plugins
    mkdir ./SAM/Qt
    echo [Paths] > ./SAM/qt.conf
    echo Plugins=./Qt >> ./SAM/qt.conf
    cp -rf /mingw64/share/qt6/plugins/* ./SAM/Qt
    
    # Install root Qt DLLs
    ldd ./SAM/SAM"$exeExt" | awk '{print $3}' > allDLLs.txt
    ldd ./SAM/Plugins/Sequential_Archive"$dllExt" | awk '{print $3}' >> allDLLs.txt
    grep -v "/Windows" allDLLs.txt > nonWindows.txt
    sort nonWindows.txt | uniq > requiredDLLs.txt
    while IFS= read -r requiredDLL; do
        cp "$requiredDLL" ./SAM/
    done < requiredDLLs.txt
    rm allDLLs.txt nonWindows.txt requiredDLLs.txt
fi
echo Done!

# Create the Deployed Files folder
echo ""; echo "Deploying Files..."

# Zip up SAM archive for users who don't want an installer
echo ""; echo "Creating 7zip archive..."
mv ./SAM/ "./SAM $version"
7z a "./SAM.$version.7z" "./SAM $version"
mv "./SAM $version" ./SAM/

# Windows Setup.exe section
#======================================================================================================================#
if [ ${MSYSTEM} == "MINGW64" ]; then
    # Prepare Installer for Compilation
    echo ""; echo "Compiling Sequential_Archive Installer..."
    startRmdirSectionMarker="# --------------- BEGIN AUTO-GENERATED RMDIR SECTION --------------- #"
    endRmdirSectionMarker="# ---------------- END AUTO-GENERATED RMDIR SECTION ---------------- #"
    startFileSectionMarker="# --------------- BEGIN AUTO-GENERATED FILE SECTION ---------------- #"
    endFileSectionMarker="# ---------------- END AUTO-GENERATED FILE SECTION ----------------- #"

    startDeleteSectionMarker="# -------------- BEGIN AUTO-GENERATED DELETE SECTION --------------- #"
    endDeleteSectionMarker="# --------------- END AUTO-GENERATED DELETE SECTION ---------------- #"
    startUninstallSectionMarker="# ------------- BEGIN AUTO-GENERATED UNINSTALL SECTION ------------- #"
    endUninstallSectionMarker="# -------------- END AUTO-GENERATED UNINSTALL SECTION -------------- #"

    installerFile="$localSourceCodeLocation"/Sequential_Archive/Sequential_Archive_Manager/Build_Scripts/Windows/Installer/SAM.nsi
    rm rmdirSection.txt fileSection.txt deleteSectionUnsorted.txt deleteSection.txt uninstallSectionUnsorted.txt uninstallSection.txt > /dev/null 2>&1

    # Prepare RMDIR Section
    find ./SAM -maxdepth 1 -type d -print > directories.txt
    sed -i 's|/|\\|g' directories.txt
    sed -i 's|\.\\SAM|\$INSTDIR|g' directories.txt
    sed -i '/\\Config/d' directories.txt # don't delete config directories
    while IFS= read -r directory; do
        echo RmDir /r \""$directory"\" >> rmdirSection.txt
    done < directories.txt
    sed -i '/\"$INSTDIR\"/d' directories.txt # don't delete the base directory
    sed -i '/^RmDir \/r "$INSTDIR"$/d' rmdirSection.txt

    # Prepare File Section
    deployLocation="$(pwd)/SAM"
    find ./SAM -type d -print > directories.txt
    while IFS= read -r directory; do
        echo SetOutPath \""$directory"\" >> fileSection.txt
        while IFS= read -r -d $'\0' file; do
            echo File \""$file"\" >> fileSection.txt
        done < <(find "$directory" -maxdepth 1 -type f -print0)
    done < directories.txt
    sed -i 's|SetOutPath \"\./SAM|\SetOutPath \"$INSTDIR|g' fileSection.txt
    sed -i 's|File \"./SAM/|File \"'"$deployLocation"'/|g' fileSection.txt

    # Prepare Delete Section
    find ./SAM -type f -print > files.txt
    sed -i 's|/|\\|g' files.txt
    sed -i 's|\.\\SAM|$INSTDIR|g' files.txt
    while IFS= read -r file; do
        echo Delete \""$file"\" >> deleteSectionUnsorted.txt
    done < files.txt
    sed -i '/SAM\.exe/d' deleteSectionUnsorted.txt # don't delete the SAM exe for now
    awk '{ print length($0) " " $0; }' deleteSectionUnsorted.txt | sort -n -r | cut -d ' ' -f 2- > deleteSection.txt # sort from longest to shortest

    # Prepare Uninstall Section
    find ./SAM -type d -print > directories.txt
    sed -i 's|/|\\|g' directories.txt
    sed -i 's|\.\\SAM|\$INSTDIR|g' directories.txt
    while IFS= read -r directory; do
        echo RmDir /r \""$directory"\" >> uninstallSectionUnsorted.txt
    done < directories.txt
    sed -i '/\"$INSTDIR\"/d' directories.txt # don't delete the base directory
    sed -i '/^RmDir \/r "$INSTDIR"$/d' uninstallSectionUnsorted.txt
    awk '{ print length($0) " " $0; }' uninstallSectionUnsorted.txt | sort -n -r | cut -d ' ' -f 2- > uninstallSection.txt # sort from longest to shortest

    convert_unix_paths_to_windows() {
        sed -i 's|/a/|A:/|g; s|/b/|B:/|g; s|/c/|C:/|g; s|/d/|D:/|g; s|/e/|E:/|g; s|/f/|F:/|g; s|/g/|G:/|g; s|/h/|H:/|g; s|/i/|I:/|g; s|/j/|J:/|g; s|/k/|K:/|g; s|/l/|L:/|g; s|/m/|M:/|g; s|/n/|N:/|g; s|/o/|O:/|g; s|/p/|P:/|g; s|/q/|Q:/|g; s|/r/|R:/|g; s|/s/|S:/|g; s|/t/|T:/|g; s|/u/|U:/|g; s|/v/|V:/|g; s|/w/|W:/|g; s|/x/|X:/|g; s|/y/|Y:/|g; s|/z/|Z:/|g; s|/|\\|g' "$1"
    }

    convert_unix_paths_to_windows "fileSection.txt"
    convert_unix_paths_to_windows "deleteSection.txt"

    update_section() {
        # Find the line numbers of the markers
        startLine=$(grep -n "$1" "$installerFile" | cut -d ':' -f 1)
        endLine=$(grep -n "$2" "$installerFile" | cut -d ':' -f 1)

        # Check if markers were found
        if [ -z "$startLine" ] || [ -z "$endLine" ]; then
          echo "Markers not found in $installerFile"
          exit 1
        fi

        # Calculate the line range to delete
        deleteRange="$((startLine+1)),${endLine}d"
        
        # Restore the end section marker
        echo $2 >> $3

        # Delete lines between markers and insert contents of fileSection.txt
        sed -i "$deleteRange" "$installerFile"
        sed -i "${startLine}r $3" "$installerFile"
    }

    update_section "$startRmdirSectionMarker" "$endRmdirSectionMarker" "rmdirSection.txt"
    update_section "$startFileSectionMarker" "$endFileSectionMarker" "fileSection.txt"
    update_section "$startDeleteSectionMarker" "$endDeleteSectionMarker" "deleteSection.txt"
    update_section "$startUninstallSectionMarker" "$endUninstallSectionMarker" "uninstallSection.txt"
    rm rmdirSection.txt fileSection.txt deleteSectionUnsorted.txt deleteSection.txt uninstallSectionUnsorted.txt uninstallSection.txt directories.txt files.txt

    convert_to_windows_path() {
        echo "$1" | sed 's|/a/|A:/|g; s|/b/|B:/|g; s|/c/|C:/|g; s|/d/|D:/|g; s|/e/|E:/|g; s|/f/|F:/|g; s|/g/|G:/|g; s|/h/|H:/|g; s|/i/|I:/|g; s|/j/|J:/|g; s|/k/|K:/|g; s|/l/|L:/|g; s|/m/|M:/|g; s|/n/|N:/|g; s|/o/|O:/|g; s|/p/|P:/|g; s|/q/|Q:/|g; s|/r/|R:/|g; s|/s/|S:/|g; s|/t/|T:/|g; s|/u/|U:/|g; s|/v/|V:/|g; s|/w/|W:/|g; s|/x/|X:/|g; s|/y/|Y:/|g; s|/z/|Z:/|g; s|/|\\\\|g'
    }

    # Update the Installer version
    sed -i "s/!define VERSION \".*/!define VERSION \"$installerVersion\"/g" "$installerFile"

    # Update the license location
    cp -f "$localSourceCodeLocation/Sequential_Archive/LICENSE" "./LICENSE.txt"
    licenseLocation=$(convert_to_windows_path "$(pwd)/LICENSE.txt")
    sed -i "s|^!define LICENSE_TXT \".*\"|!define LICENSE_TXT \"$licenseLocation\"|" "$installerFile"

    # Update the installer output location
    installerOutputLocation=$(convert_to_windows_path "$(pwd)/SAM.$version.Setup.exe")
    sed -i "s|^!define INSTALLER_NAME \".*\"|!define INSTALLER_NAME \"$installerOutputLocation\"|" "$installerFile"

    # Compile the Sequential_Archive installer for users who want an installer
    makensis "$installerFile"
    rm "./LICENSE.txt"
fi

echo "Done!"; echo ""; echo "Deploy complete! Enjoy SAM!"
exit 0
