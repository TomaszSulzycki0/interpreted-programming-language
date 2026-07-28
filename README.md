
# Interpreted programming language

A tree-walk interpreter for a custom, statically typed programming language, built from scratch in C++ using CMake.


## Features

- Data types: int, bool, float, double, string.
- Control flow: if statements, while statements, scoping. 
- Functions: Simple functions bound to identifiers.
- Comments.

## Installation

### 1. Clone repository
```bash
git clone https://github.com/TomaszSulzycki0/interpreted-programming-language.git
```
### 2. Navigate to directory
```bash
cd your-repo-name
```

### 3. Configure CMake build directory
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

### 4. Build the executable
```bash
cmake --build build
```
    
## Usage

### Running the interpreter

#### 1. Description mode

```bash
./build/Release/ipl
```

#### 2. Run a script

```bash
./build/Release/ipl path/to/script.ipl
```

### Code example

```bash
fn foo(int x) -> bool
{
    return x < 5;
}

int i = 0;
float f = 1.0;
double d = 3.14;
bool b = true;
string s = "ipl";

if ( b )
{
    while ( foo(i) )
    {
        s +=  "-" + s;
        i += 1;
    }
}
```