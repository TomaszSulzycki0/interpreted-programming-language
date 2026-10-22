

[![CI](https://img.shields.io/github/actions/workflow/status/TomaszSulzycki0/interpreted-programming-language/ci.yml?style=for-the-badge&label=CI&logo=github)](https://github.com/TomaszSulzycki0/interpreted-programming-language/actions/workflows/ci.yml) [![Live Demo](https://img.shields.io/badge/Live%20Demo-Open%20Playground-4f46e5?style=for-the-badge&logo=webassembly&logoColor=white)](https://tomaszsulzycki0.github.io/interpreted-programming-language/)

# Interpreted programming language

A tree-walk interpreter for a custom statically typed programming language, built from scratch in C++ with CMake.

## 🚀 Try it online

You can run IPL directly in your browser, with no installation needed. The interpreter is compiled to WebAssembly with Emscripten, so your code runs locally and nothing is sent to a server.

**[Open the interactive playground →](https://tomaszsulzycki0.github.io/interpreted-programming-language/)**

The playground includes a syntax quick reference and a few example programs to get you started.
## Features

- Data types: int, bool, float, double, string.
- Control flow: if-else statements, while statements, functions. 
- Functions: scope isolation, recursion.
- Built-in functions: print, casting functions
- Objects: Storing multiple variables
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