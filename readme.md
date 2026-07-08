# if on windows
Open this in UCRT64 Shell
./build_obby.sh for the latest version

# if on linux
- if you edited the CMakeLists.txt file in balls:
	- you need to `cmake -S . -B build --fresh ` to get rid of previous build structure
- to build:
	- `cmake --build build` to create new build environment

# each iteration of the project is in its own directory now
- this was so cmake structure was cleaner
- so far only main and course work (i haven't tried others)
