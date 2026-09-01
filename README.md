
# Interpreted programming language

A tree-walk interpreter for a custom, statically typed programming language, built from scratch in C++ using CMake.


## Features

- Data types: int, bool, float, double, string.
- Control flow: if-else statements, while statements, functions. 
- Functions: scope isolation, recursion.
- Built in functions: print, casting functions
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