# EdgeX Honeycomb GUI

A modern, responsive C++ Qt application for managing EdgeX Foundry instances with a premium Honeycomb-inspired theme.

## Features
- **Dynamic Service Discovery**: Real-time monitoring of all EdgeX services via Consul registry.
- **Premium Aesthetics**: Honeycomb Navy (#023356) and Orange (#F9942A) palette with Montserrat typography.
- **Service Management**: Dashboard, Device Management, Rules Engine, and more.

## Prerequisites

Ensure you have the following installed:
- **Qt 5.15+ or Qt 6** (Widgets, Network, Concurrent modules)
- **CMake 3.10+**
- **C++17 Compiler** (GCC, Clang, or MSVC)
- **Consul** (Required for service discovery, usually running at `http://localhost:8500`)

## How to Build

Follow these steps to build the application from source:

1. **Navigate to the project directory:**
   ```bash
   cd edgex-gui
   ```

2. **Create and enter the build directory:**
   ```bash
   mkdir build && cd build
   ```

3. **Configure the project with CMake:**
   ```bash
   cmake ..
   ```

4. **Build the application:**
   ```bash
   make -j$(nproc)
   ```

## How to Run

After a successful build, you can run the application using the following commands:

### Linux / WSL
If you are using WSL, ensure you have an X-Server or Wayland compositor running.

```bash
export DISPLAY=:0  # Adjust if your display index is different
cd build
./bin/edgex-qt-ui
```

### Windows
Navigate to the `build/bin` directory and run:
```powershell
.\edgex-qt-ui.exe
```

## Configuration

The application stores settings in a `config.json` file located in the user's application config directory. You can manually adjust the service URLs or the Consul registry URL there, or use the **Settings** dialog within the app.

- **Default Registry URL**: `http://localhost:8500`

## Interface Guidelines

- **Typography**: The app uses **Montserrat**. Ensure this font is installed on your system for the best experience.
- **Logo**: The sidebar logo is located in `src/resources/honeycomblogo.png` and is embedded via the Qt Resource System (`.qrc`).
