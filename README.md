# PSZ Homebrew Ports

Multi-platform homebrew ports of **PSZ** (Encrypted Project Archives) for **Nintendo 3DS**, **Nintendo Wii U**, **Nintendo Wii**, and **Sony PSP**.

## Overview

This repository provides C-based ports and build configurations for the PSZ archive extraction engine across classic and modern gaming consoles.

| Platform | Target File Format | Toolchain / SDK |
|----------|-------------------|-----------------|
| **Sony PSP** | `.pbp` / `.elf` | PSPSDK |
| **Nintendo 3DS** | `.3dsx` | devkitARM (`libctru`) |
| **Nintendo Wii** | `.dol` | devkitPPC (`libogc`) |
| **Nintendo Wii U** | `.rpx` | devkitPPC (`wiiu-rules`) |

---

## Project Structure

```text
psz-ports/
├── include/
│   └── psz.h              # Shared core header for PSZ engine
├── src/
│   └── psz_core.c         # Core extraction and archive handling logic
├── platforms/
│   ├── psp/               # PlayStation PSP source & Makefile
│   ├── 3ds/               # Nintendo 3DS source & Makefile
│   ├── wii/               # Nintendo Wii source & Makefile
│   └── wiiu/              # Nintendo Wii U source & Makefile
└── .github/
    └── workflows/
        └── build.yml      # Automated GitHub Actions CI workflow
```

---

## Building Locally

### Prerequisites
- **devkitPro** (for 3DS, Wii, and Wii U) with `devkitARM` and `devkitPPC` installed.
- **PSPSDK** (for PlayStation PSP).

### Compilation Commands

- **PSP**:
  ```bash
  cd platforms/psp
  make
  ```
- **Nintendo 3DS**:
  ```bash
  cd platforms/3ds
  make
  ```
- **Nintendo Wii**:
  ```bash
  cd platforms/wii
  make
  ```
- **Nintendo Wii U**:
  ```bash
  cd platforms/wiiu
  make
  ```

---

## License

MIT – see LICENSE for details.
