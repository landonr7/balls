# install cmake
# Have not tested this yet
# if on windows
Open this in UCRT64 Shell
./build_obby.sh for the latest version

# if on linux
- if you edited the CMakeLists.txt file in balls:
	- you need to `cmake -S . -B build --fresh` to get rid of previous build structure
- to build:
	- `cmake --build build` to create new build environment
- to execute:
	- `./build/<parent directory name>`

# each iteration of the project is in its own directory now
- this was so cmake structure was cleaner
- so far only obby and course work (i haven't tried main)
- "obby" is the playround for developing an obstacle in an isolated environment
- "course" is where the final product is intended to be.
