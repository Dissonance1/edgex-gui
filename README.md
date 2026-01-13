# EdgeX Honeycomb GUI

A modern, responsive C++ Qt application for managing EdgeX Foundry instances with a premium Honeycomb-inspired theme.

## Features
- **Dynamic Service Discovery**: Integrates with **Consul** to automatically discover and monitor all running EdgeX services.
- **EdgeX V3 Parity**:
  - **Notifications**: Severity-based monitoring and multi-channel subscription support.
  - **Rules Engine**: SQL-based stream processing with real-time metrics (In/Out/Exceptions).
- **Premium Aesthetics**: High-fidelity UI using the Honeycomb palette (#023356, #F9942A) and **Montserrat** typography.
- **Responsive Management**: Full CRUD operations for Devices, Profiles, and Subscriptions.

## Prerequisites

Ensure you have the following installed:
- **Qt 5.15+ or Qt 6** (Modules: Widgets, Network, Concurrent)
- **CMake 3.10+**
- **C++17 Compiler** (GCC, Clang, or MSVC)
- **Consul**: Required for service discovery (Default: `http://localhost:8500`)

---

## How to Build

Follow these steps to build the application from source:

1. **Navigate to the project directory:**
   ```bash
   cd edgex-gui
   ```

2. **Setup Build Environment:**
   ```bash
   mkdir build && cd build
   ```

3. **Configure & Compile:**
   ```bash
   cmake ..
   make -j$(nproc)
   ```

---

## How to Run

### Linux / WSL
If using WSL, ensure an X-Server (like GWSL or VcXsrv) is running on your host.

```bash
# In the project root:
export DISPLAY=:0
./build/bin/edgex-qt-ui
```

### Windows
Navigate to the `build\bin` folder and execute:
```powershell
.\edgex-qt-ui.exe
```

---

## Troubleshooting

- **"No such file or directory" when running**: Ensure you are in the correct directory. If you are already inside `build`, use `./bin/edgex-qt-ui`.
- **Display Errors (WSL)**: If the app fails to open, verify your `DISPLAY` variable. Use `export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2}'):0` if `:0` doesn't work.
- **Consul Connection**: If System Status is empty, ensure Consul is running and accessible at the URL defined in **Settings**.

## Configuration

The application stores settings in a `config.json` file.
- **Linux**: `~/.config/edgex-qt-ui/config.json`
- **Windows**: `AppData\Local\edgex-qt-ui\config.json`

## Interface Guidelines

- **Font**: Montserrat (Included in system or available via Google Fonts).
- **Branding**: Logos and styles are maintained in `src/resources/` and `src/ModernTheme.qss`.
