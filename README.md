# Typed Lua Wrapping

A library for creating bindings between Lua and C++ in a typed manner.

## Getting Started

1. To get started, firstly clone this repository and get into the project directory.
2. Create a build directory and go to the build directory.
`mkdir build; cd build`
3. Configure and build the project.
`cmake [options] ..; make`.
Options are:
   * `-DBUILD_TESTS=ON` (Default ON)
   * `-DBUILD_BENCHMARKS=OFF` (Default OFF)
   * `-DCMAKE_BUILD_TYPE=Release` (Default Debug)
4. Running some executables:
   * `./tlw-tests` (running the tests)
   * `./tlw-benchmarks` (running the benchmarks)

### Prerequisites

Only one library, LuaJIT, is required if you skip the tests.

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

### Uninstalling

The CMake project will create an uninstall target. Simply go to your build directory and
execute `make uninstall`.

## Running the tests

The tests in this project are meant to protect the use cases often found in embedding with Lua.
It'll test all stack operations, including argument checks when built in Debug,
like double, int, string, classes, e.t.c.

The creation of class concepts is roughly covered in the tests also.
To execute the tests, simply run `./tlw-tests` in the build directory.

## Running the benchmarks

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
For the rest the proces stays the same.
```bash
mkdir build-release; cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make install
```

## Built With

* [GCC](https://gcc.gnu.org/) - The compiler (MS Visual C++ not tested)
* [CMake](https://cmake.org/) - Used for maintaining the sourcecode project
* [LuaJIT](http://luajit.org/) - The library that is extended
* [GTest](https://github.com/google/googletest) - The library for testing

## Examples

These examples will demonstrate how to use the library in your own project. You can always consult
the tests and benchmarks for more examples.

### Add to CMake project

After the project is built and installed (*see § Installing*) you can use it in your own CMake project.
```cmake
find_library(TLW REQUIRED)

add_executable(my-executable main.cpp)
target_link_libraries(my-executable TLW)
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
#include <tlw/types.hpp>

int main(int argc, char** argv) {
    Lua lua;
    auto script = lua.file<LuaFunction<LuaFunction<double>(double, double)>()>("addup.lua");
    auto func = script();
    return func(2.5, 2.5);
}
```

As you can see the script is obtained in the same manner as the function.
The function signature is very imported in this example, as the signature
has to meet the actual script file or function.

Note that for a multi return function/script you can use an `std::tuple`.

### Exposing globals

For exposing globals I will only demonstrate the C++ code.

```cpp
#include <tlw/tlw.hpp>

const unsigned int myInteger = 200;
const double myNumber = 2.5;
const char * myString = "Lorem Ipsum";


int main(int argc, char** argv) {
    Lua lua;
    // These values will be available globally in a script
    // with the same lua_State of course
    lua.global("myInteger", myInteger);
    lua.global("myNumber", myNumber);
    lua.global("myString", myString);
    return 0;
}
```

There are various types that can be exposed/fetched. For example:
 * `T*` Which will either result in a user datum, or if the class isn't known it will result in a light user datum.
 * Primitives: `bool`, `int`, `unsigned int`, `float`, `double`
 * Strings: `char *`, `const char *`, `std::string`
 * List: `std::vector`
 * Multi type list: `std::tuple`

By fetching tables and lists you won't have enough flexibility with the last structure types.
Therefore there's an `LuaTable` and `LuaList`.
You can manipulate or fetch the contents of these classes easily.

### Exposing classes

This example will demonstrate how a class is exposed to lua.
It will be compatible with the following Lua example.
```lua
-- File: myclass.lua
print(example:getNumber())
example:setNumber(5.0)
print(example:getNumber())
```

```cpp
#include <tlw/tlw.hpp>
#include <tlw/types.hpp>
#include <tlw/wrapping.hpp>

class MyClass {
private:
    double myNumber = 0;
public:
    void set(double num) { myNumber = num; }
    double get() { return myNumber; }
}

int main(int argc, char **argv) {
    Lua lua;
    MyClass myClass;
    lua.add<MyClass>(ClassPrototypeBuilder("MyClass")
                    .method("setNumber", mk_function(&MyClass::set))
                    .method("getNumber", mk_function(&MyClass::get))
                    .build());
    lua.global("example", &myClass)
       .file("myclass.lua")();
}
```

Note that for the 'normal' class prototype the `mk_function` macro is used i.o. the `mk_method` macro.
If you're against the use of macro's this one is avoided by typing the following:
```cpp
...
    .method("setNumber", [] (lua_State *L)->int { return luaWrapFunction(L, &MyClass::set); })
...
```


### Exposing pretty classes

This section will cover the pretty outcome of a `PrettyClassPrototype`.
The exposing is almost the same as the previous section,
yet it will behave pretty different in lua code.

```lua
-- File: myclass.lua
print(example.number)
example.number = 5.5
print(example.getNumber)
example.print('The number is ')
```

```cpp
#include <tlw/tlw.hpp>
#include <tlw/types.hpp>
#include <tlw/wrapping.hpp>

class MyClass {
public:
    double myNumber = 0;
    void set(double num) { myNumber = num; }
    double get() { return myNumber; }
    void print(const char * prefix) {
        std::cout << prefix << myNumber << std::endl;
    }
}

int main(int argc, char **argv) {
    Lua lua;
    MyClass myClass;
    lua.add<MyClass>(PrettyClassPrototypeBuilder("MyClass")
                    .property("number", mk_property(&MyClass::number))
                    .setter("setNumber", mk_function(&MyClass::set))
                    .getter("getNumber", mk_function(&MyClass::get))
                    .method("printNumber", mk_method(&MyClass::print))
                    .build());
    lua.global("example", &myClass)
       .file("myclass.lua")();
}
```

Note that the getter/setter functions will behave the same as a property and
are bound using the `mk_function` macro. Also note that the property
uses the `mk_property` macro. Though, the property example will execute faster,
yet you should think twice before setting properties in C++ public. At last
we can see that the method also doesn't require a `:` to be executed.
For this exposure we'll need to use the `mk_method` macro.

The difference between the macro's have some technical background. In short,
the `mk_method` will expect a user datum by upvalue rather than the first function argument.

## Contributing

Please feel free to address an issue or create a pull request directly.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/MacLotsen/tlw/tags). 

## Authors

* **Erik Nijenhuis** - *Initial work* - [MacLotsen](https://github.com/MacLotsen)

See also the list of [contributors](https://github.com/MacLotsen/tlw/contributors) who participated in this project.

## License

This project is licensed under the GPLv2 License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* For CMake install targets I consulted [this article](https://cliutils.gitlab.io/modern-cmake/chapters/install/installing.html).
* The FindLuaJIT.cmake file is copied from [minetest](https://gitlab.com/minetest/minetest)
* For the template like interface I got inspired by [luaaa](https://github.com/gengyong/luaaa)
* For this readme I was inspired by the template of [PurpleBooth](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2)
