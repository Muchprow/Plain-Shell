# 🐚 Plain Shell

**Plain Shell** is a lightweight, customizable, and portable command-line shell written in C++17. Designed with modularity in mind, featuring custom theme addons, command packs, and built-in shortcuts.

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey.svg)
![License](https://img.shields.io/badge/license-GPL--3.0-green.svg)

---

## ✨ Features

- 🎨 **Addons System (`.plainmod`)**: Custom prompts, colors, welcome screens, and startup hooks.
- 📦 **Command Packs (`.plainc`)**: Group your shortcuts into custom packs with descriptions integrated into `help`.
- 🎒 **Portable Mode**: Create a `portable/` directory next to the binary, and Plain will run 100% off your USB drive.
- ⚡ **System Integration**: Easily add Plain Shell to your User `PATH` with `setup path`.
- 📜 **History Manager**: Automatically logs and manages your command history.

---

## 🛠️ Building from Source

### Prerequisites
- C++17 compatible compiler (GCC, Clang, or MSVC)
- CMake 3.15+

---

## 💖 Support & Donations

If you like **Plain Shell** and want to support its development, you can donate via EVM / MetaMask (ETH, USDT, USDC, BNB, MATIC, etc.):

**MetaMask (EVM Address):**
`0x4B4d23cdeD10F5585B219066127422c62D8D09C6`

*Thank you for your support! 🚀*

---

### Build Steps, Quick Start & Usage
```bash
git clone [https://github.com/Muchprow/Plain-Shell.git](https://github.com/Muchprow/Plain-Shell.git)
cd plain-shell
mkdir build && cd build
cmake ..
cmake --build .

# View available commands and loaded pack commands
plain> help

# Manage Addons / Themes
plain> addons enable matrix
plain> addons disable

# Manage Command Packs
plain> packs enable git_pack

# Register Plain to System PATH (Windows)
plain> setup path
