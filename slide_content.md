# Unified Multi-Platform Architecture for Console Homebrew

## Modular C-Core Enables Seamless Cross-Platform Porting Across Legacy and Modern Consoles
- Decoupling core archive extraction and packing logic from hardware-specific APIs ensures high maintainability and consistent behavior across Nintendo and Sony architectures.
- Standardized header structures (`psz_header_t`) manage multi-format support (PSZ, ZIP, TAR) with zero platform-specific overhead.
- Centralized filesystem abstraction allows uniform handling of storage devices (`sdmc:/`, `ms0:/`, `fs:/vol/external01`).

## Hardware-Accelerated Citro2d Rendering Transforms Nintendo 3DS User Experience
- Transitioning from legacy text consoles to GPU-accelerated Citro2d provides crisp, 60 FPS dual-screen rendering.
- Dedicated top screen dashboard displays real-time operational status and interactive instructions.
- Bottom screen touch-enabled file browser supports intuitive directory navigation and instant file selection.

## Automated CI/CD Pipeline with Integrated Unit Testing Guarantees Code Reliability
- Automated test suites validate format detection, archive creation, and extraction logic on every GitHub push.
- Dedicated containerized environments (`pspdev`, `devkitARM`, `devkitPPC`) compile optimized binaries (`.3dsx`, `.dol`, `.rpx`, `.pbp`) in parallel.
- Automated asset bundling embeds custom high-resolution icons and cinematic banners directly into target executables.

## Comprehensive Multi-Console Ecosystem Delivers Unified Archive Management
- Uniform feature parity across Nintendo 3DS, Wii U, Wii, and PlayStation PSP bridges disparate hardware generations.
- Touchscreen and hardware button abstractions provide responsive controls tailored to each console's input capabilities.
- Open-source GitHub repository serves as a robust foundation for community-driven homebrew expansion.
