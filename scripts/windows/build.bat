
ECHO "Checking for build directory..."

IF exist ".\build\" (
	ECHO "build directory already exists."
	ECHO "Removing..."
	RMDIR /S /Q build
)

ECHO "Create build directory"
MKDIR build

ECHO "Moving into build directory"
CD build

ECHO "Enabling MSVC Dev Tools"
REM CALL "%MSVCTools%/VsDevCmd.bat"

ECHO "Generating build environment..."
cmake ..

ECHO "Making..."
cmake --build . --config Release
