# Stupid Brick Breaker

A C++ brick breaker/breakout game built with [raylib](https://www.raylib.com/) - a simple and easy-to-use library for game development.

## About

Stupid Brick Breaker is a classic brick breaker game where you control a paddle to bounce a ball and destroy bricks on the screen. Try to clear all the bricks and achieve the highest score!

## Requirements

Before building the game, make sure you have the following installed:

### macOS
- **Xcode Command Line Tools** - Required for compilation
  ```bash
  xcode-select --install
  ```
- **CMake** (version 3.10 or higher)
  ```bash
  brew install cmake
  ```
- **raylib** - Graphics library
  ```bash
  brew install raylib
  ```

### Linux (Ubuntu/Debian)
```bash
sudo apt-get install build-essential cmake libraylib-dev
```

### Windows
- **Visual Studio** or **MinGW** with C++ development tools
- **CMake** from https://cmake.org/download/
- **raylib** from https://github.com/raysan5/raylib/releases or through package managers

## Installation & Building

### Clone the Repository
```bash
git clone <your-repo-url>
cd Stupid-Brick-Breaker
```

### Build with CMake

1. **Create a build directory:**
   ```bash
   mkdir build
   cd build
   ```

2. **Configure the project:**
   ```bash
   cmake ..
   ```

3. **Build the executable:**
   ```bash
   make
   ```
   
   Or on Windows with Visual Studio:
   ```bash
   cmake --build . --config Release
   ```

### Running the Game

After building, run the executable:

**macOS/Linux:**
```bash
./Untitled_Game
```

**Windows:**
```bash
Untitled_Game.exe
```

Or from the build directory:
```bash
./build/Untitled_Game
```

## Project Structure

```
Stupid-Brick-Breaker/
├── CMakeLists.txt          # CMake build configuration
├── main.cpp                # Main game source code
├── raylib-cpp/             # raylib C++ wrapper library
└── build/                  # Build output directory (created after building)
```

## Game Controls

- **Mouse**: Move your paddle across the bottom of the screen
- **Game Menu**: Click buttons to start or navigate
- **Pause**: Press the pause button during gameplay

## Troubleshooting

### Build Fails with "raylib.h not found"
Make sure raylib is properly installed:
- **macOS**: `brew install raylib`
- **Linux**: `sudo apt-get install libraylib-dev`
- **Windows**: Download from https://github.com/raysan5/raylib

### CMake Can't Find raylib
On macOS, if using Homebrew, raylib is typically installed in `/opt/homebrew/opt/raylib/`. The CMakeLists.txt is configured to look there by default.

### Port Issues or Runtime Errors
If the game crashes with exit code 139, ensure your raylib installation is compatible with your system and properly linked.

## Building from Source (Advanced)

If you want to build raylib from source instead of using the package manager:

1. Clone raylib: `git clone https://github.com/raysan5/raylib.git`
2. Follow building instructions at https://github.com/raysan5/raylib/wiki/Build-and-Installation
3. Update the `CMakeLists.txt` paths to point to your custom raylib build

## Development

The game is written in C++ with the following key components:
- Game state management (MENU, PLAYING, GAME_OVER, etc.)
- Ball physics and collision detection
- Paddle control
- Score tracking
- Multiple game states and transitions

## License

See the [LICENSE](LICENSE) file for details.

## Contributing

Feel free to fork this repository and submit pull requests with improvements!

## Further Help

For issues with raylib, visit: https://www.raylib.com/
For CMake help, visit: https://cmake.org/cmake/help/latest/
