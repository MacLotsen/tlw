# Typed Lua Wrapping

A library for creating bindings between Lua and C++ in a typed manner.

## Getting Started

1. To get started, firstly obtain the software by either of the options.

    a. Clone from the repository:<br>
       `git clone https://github.com/MacLotsen/tlw.git; cd tlw`

    b. Download the source and extract:<br>
       ```bash
       curl -Lo tlw.zip https://github.com/MacLotsen/tlw/archive/master.zip
       unzip tlw.zip; cd tlw-master
       ```

2. Create a build directory and go to the build directory.

    `mkdir build; cd build`

3. Configure and build the project.

   `cmake [definitions] ..; make`.
   
   Definitions to use:
   * `-DBUILD_TESTS=ON` (Default ON)
   * `-DBUILD_BENCHMARKS=OFF` (Default OFF)
   * `-DCMAKE_BUILD_TYPE=Release` (Default Debug)

4. Running some executables:

   `./tlw-tests` (running the tests)

   `./tlw-benchmarks` (running the benchmarks)
   
5. Running with LuaJit:

   The testing directory in the build directory contains an example library.
   Make sure to be in the build/test directory when executing.
   
   ```
   cd build/test
   luajit -i -l libtlw-examplelib lua/setup.lua
   ```

### Prerequisites

Only one library, LuaJit, is required if you skip the tests.

* MinGW (MSys2):
    ```bash
    pacman -S mingw64/mingw-w64-x86_64-luajit \
              mingw64/mingw-w64-x86_64-gtest
    ```
* Ubuntu 18.04:
    ```bash
    apt install luajit \
                libgtest-dev
    ```

### Installing

The project is easily installable. Just follow these brief instructions to install this project.
```bash
# In the project directory
mkdir build-release
cd build-release

# For now we're only interested in installing the project,
# rather than running tests e.g.
cmake -DUSE_TESTS=OFF -DCMAKE_BUILD_TYPE=Release ..

# Install it in dist directory i.o. /usr or /usr/local
# Ommit the DESTDIR if you want to install it system wide
make DESTDIR=dist/ install
```

<sub><sup>You might need root privileges</sub></sup>

### Uninstalling

The CMake project will create an uninstall target. Simply go to your build directory and
execute `make uninstall`.

<sub><sup>You might need root privileges</sub></sup>

## Running the tests

The tests in this project are meant to protect the use cases often found in embedding with Lua.
It'll test all stack operations, including argument checks when built in Debug,
like double, int, string, classes, e.t.c.

The creation of class concepts is roughly covered in the tests also.
To execute the tests, simply run `./tlw-tests` in the output directory.

## Running the benchmarks

To run the benchmarks; go to the output directory and run `./tlw-benchmarks`.

The benchmarks are showing the difference in time between TLW concrete implementations
and Lua implementations.

The Lua implementations will execute the same script as the TLW implementations will do.
Therefore different meta tables are implemented i.e. for `example.property = 'prop'` and
`example.property('prop')`. Yet, sometimes the Lua implementations will cheat,
because the developer would have known all functions when creating the meta table, therefore
skipping upvalue logic used in `PrettyClassPrototype`, making it faster.

So the benchmarks will not show an exact copy between TLW and manually binding, but it will
show the time lost when using a specific TLW implementation.
If you can come up with a faster lua implementation, please feel free to create a pull request.

## Deployment

When deploying to a target, make sure to set the option `CMAKE_BUILD_TYPE`.
For the rest the process stays the same.
```bash
mkdir build-release; cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make install
```

## Built With

* [GCC](https://gcc.gnu.org/) - The compiler (MS Visual C++ not tested)
* [CMake](https://cmake.org/) - Used for maintaining the sourcecode project
* [LuaJit](https://luajit.org/) - The library that it extends
* [GTest](https://github.com/google/googletest) - The library for testing

## Examples

These examples will demonstrate how to use the library in your own project. You can always consult
the tests and benchmarks for more examples.

### Add to CMake project

After the project is built and installed (*see § Installing*) you can use it in your own CMake project.
```cmake
find_library(TLW REQUIRED)

add_executable(my-executable main.cpp)
target_link_libraries(my-executable tlw)
```
And you're up and running!

### Executing Lua Scripts and Functions
This section will demonstrate a simple script which will return a function.
The actual Lua function will be executed by C++.

The Lua code:
```lua
-- File: addup.lua
local function addUp(a, b)
    return a + b
end

return addUp
```

The C++ code:
```cpp
#include <tlw/tlw.hpp>

int main(int argc, char** argv) {
    using my_func = tlw::function<double(double, double)>;
    tlw::lua lua;
    tlw::function<my_func()> script = lua.file<my_func>()>("addup.lua");
    my_func f = script();
    return f(2.5, 2.5);
}
```

### Assigning globals

This C++ code will demonstrate how globals can be assigned.

```cpp
#include <tlw/tlw.hpp>

unsigned int myInteger = 200;
double myNumber = 2.5;
const char * myString = "Lorem Ipsum";


int main(int argc, char** argv) {
    tlw::lua lua;
    // These values will be available globally in a script
    // with the same lua_State of course
    lua.set("myInteger", myInteger);
    lua.set("myNumber", myNumber);
    lua.set("myString", myString);
    return 0;
}
```

There are various types that can be handled. Defined types are:
 * Primitives: `bool`, `int`, `unsigned int`, `float`, `double`, `long`, `unsigned long`
 * Strings: `const char *`, `std::string`
 * Structures: `std::vector`, `std::unordered_map`, `std::array`
 * Functions: `tlw::function`, or any other function type
 * Any other type will either result in user data, or if the class isn't known it will result in light user data.

### Defining user types

This section will demonstrate how a simple class can be made available in lua.
The demonstration will be compatible with the following Lua example.

```lua
-- file: example.lua
local e1 = example(5, 'example')
e1.print() -- Name: example 5

local e2 = e1 + 1
e2.print() -- Name: example 6

e2.n = 88
print(e2.n) -- 88
```

```cpp
struct Example {
    float n;
    const char * name;
    
    Example(float n, const char * name) : n(n), name(name) {}
    
    Example operator+(float val) const {
        return Example(n+val, name);
    }

    void printName() const {
        printf("Name: %s %g", name, n);
    }
};

int main(int argc, char **argv) {
    tlw::lua lua;
    
    auto defineExample = tlw::define<Example>("example")
            .ctor<float, const char*>()
            .prop("n", &Example::n)
            .add<int>()
            .method("print", &Example::printName)
            .build();

    defineExample(lua.L);

    lua.file("example.lua")();
}
```

## Contributing

Please feel free to address an issue or create a pull request directly.

## Versioning

The version of this library uses 3 number positions separated by dots.
Optionally it'll have a post fix with the number of commits after tag leading with a short hash of the commit.

For example:
 - 0.0.1
 - 0.0.1-11-g8fdd034
 
So the second example is eleven commits ahead of the version with a short hash of `g8fdd034`.

## Authors

* **Erik Nijenhuis** - *Initial work* - [MacLotsen](https://github.com/MacLotsen)

See also the list of [contributors](https://github.com/MacLotsen/tlw/contributors) who participated in this project.

## License

This project is licensed under the GPLv2 License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* The FindLuaJIT.cmake file is copied from [minetest](https://gitlab.com/minetest/minetest)
* For the template like interface I got inspired by [luaaa](https://github.com/gengyong/luaaa)
