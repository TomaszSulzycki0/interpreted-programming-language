
[![C++ CMake CI Suite](https://github.com/TomaszSulzycki0/interpreted-programming-language/actions/workflows/ci.yml/badge.svg)](https://github.com/TomaszSulzycki0/interpreted-programming-language/actions/workflows/ci.yml)

# Interpreted programming language

A tree-walk interpreter for a custom statically typed programming language, built from scratch in C++ with CMake.


## Features

- Data types: int, bool, float, double, string.
- Control flow: if-else statements, while statements, functions. 
- Functions: scope isolation, recursion.
- Built-in functions: print, casting functions
- Comments.

## Installation

### 1. Clone repository
```bash
git clone https://github.com/TomaszSulzycki0/interpreted-programming-language.git
```
### 2. Navigate to directory
```bash
cd interpreted-programming-language
```

### 3. Configure the build
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

### 4. Build the executable
```bash
cmake --build build
```
    
## Usage

### Running the interpreter

#### 1. Interpreter description

```bash
./build/ipl
```

#### 2. Run a script

```bash
./build/ipl path/to/script.ipl
```

#### 3. Run tests

```bash
ctest --test-dir build --output-on-failure
```

### Code example

```ipl
fn RecCollatz(int x, int num_steps) -> int
{    
    if ( x == 1)
    {
        return num_steps;
    }

    if ( x % 2 == 0)
    {
        return RecCollatz( x / 2 , num_steps + 1);
    }
    else
    {
        return RecCollatz(3 * x + 1, num_steps + 1);
    }
}

int num_samples = 100;
int i = 1;

while( i < num_samples )
{
    int result = RecCollatz(i, 0);
    print( "Number of steps to reach 1 from " + string(i) + " : " + string(result) + "\n" );
    i += 1;
}
```