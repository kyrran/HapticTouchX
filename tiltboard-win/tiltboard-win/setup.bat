cd ../../external/fuzzylite
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" --fresh
echo "Building now"
devenv fuzzylite.sln /build Release
cd ../../../tiltboard-win/tiltboard-win
devenv tiltboard-win.sln /build Release