# kodtest

Uses small portions of C++11 for convenience. Sorry if it's not supported in your compiler. The only dependency apart
from STL that isn't in the repo is libcurl, which i thought would be common enough for it already to be in your computer.

Uses CMake to generate a makefile/Visual Studio Project/XCode project file.

Building is something like this on Linux:
cmake .
make

It's probably similiar on OS X and *BSD, depending on the version.

On Windows you might have to download and link to libcurl. It can be found at
http://curl.haxx.se/libcurl/

It also uses JsonCpp, but it is included.
