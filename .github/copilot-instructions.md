# CoPilot Instructions for the Exterminate Project

## 1. Project Overview

This project is a C++ application for the Raspberry Pi Pico, utilizing the Pico SDK and the BluePad library. The goal is to create a clean, maintainable, and robust application by adhering to modern C++ best practices.

This document provides instructions for GitHub Copilot to ensure consistency and high-quality code throughout the project.

## 2. Folder Structure

The project should follow this structure:

- `build/`: (Managed by CMake) Build output.
- `docs/`: Project documentation.
- `include/`: Public header files for this project's source code.
- `lib/`: External libraries. BluePad should be included here, preferably as a git submodule.
- `src/`: Source code files (`.cpp`).
- `tests/`: Unit tests.
- `copilot.md`: This file.
- `CMakeLists.txt`: Main CMake file.
- `.clang-format`: Code style configuration.
- `.gitignore`: Git ignore file.

**Instruction for Copilot:** When creating new files, place them in the appropriate directory. For example, new C++ classes should have their header in `include/` and their implementation in `src/`.

## 3. C++ Best Practices & SOLID Principles

### General Principles
- **Clean Code**: Write code that is easy to read and understand. Use meaningful names for variables, functions, and classes. Keep functions short and focused on a single responsibility.
- **Modern C++**: Use features from C++17 where appropriate. This includes `std::optional`, `std::string_view`, structured bindings, and `if constexpr`.
- **RAII (Resource Acquisition Is Initialization)**: All resources (memory, hardware peripherals like GPIO pins, timers, etc.) should be managed by classes that acquire the resource in the constructor and release it in the destructor.

### SOLID Principles
- **Single Responsibility Principle (SRP)**: A class should have only one reason to change. For example, a class that manages a sensor should not also be responsible for displaying data on an LCD.
- **Open/Closed Principle (OCP)**: Software entities (classes, modules, functions) should be open for extension but closed for modification. Use interfaces and abstraction to allow for new functionality without changing existing code.
- **Liskov Substitution Principle (LSP)**: Subtypes must be substitutable for their base types. When creating derived classes, ensure they honor the contract of the base class.
- **Interface Segregation Principle (ISP)**: Clients should not be forced to depend on interfaces they do not use. Create small, specific interfaces rather than large, general-purpose ones.
- **Dependency Inversion Principle (DIP)**: High-level modules should not depend on low-level modules. Both should depend on abstractions. Abstractions should not depend on details. Details should depend on abstractions. Use dependency injection to provide implementations to classes.

**Instruction for Copilot:** When implementing new features or refactoring code, explicitly consider which SOLID principles apply and strive to follow them.

## 4. Library Integration (BluePad)

The BluePad library should be added to the `lib/` directory as a git submodule.

To add BluePad:
```bash
git submodule add https://github.com/ricardoquesada/bluepad32.git lib/bluepad32
```

The main `CMakeLists.txt` must be updated to include the BluePad library. This typically involves adding `add_subdirectory(lib/bluepad32)`.

**Instruction for Copilot:** When asked to integrate a new library, prefer using git submodules and update the `CMakeLists.txt` file accordingly.

## 5. Code Style and Formatting

This project will use a consistent code style enforced by `.clang-format`.

- **Naming Conventions**:
    - `ClassName`: PascalCase
    - `functionName`, `variableName`: camelCase
    - `CONSTANT_NAME`, `ENUM_VALUE`: UPPER_CASE_SNAKE_CASE
- **Headers**: Use `#pragma once` for include guards.
- **Pointers**: Use `nullptr` instead of `NULL` or `0`.

**Instruction for Copilot:** All generated or modified C++ code must adhere to the style defined in the `.clang-format` file.

## 6. Testing

While setting up a full unit testing framework like GoogleTest on the Pico can be complex, writing testable code is a primary goal.

- Keep business logic separate from hardware-specific code.
- Use abstractions (interfaces) for hardware to allow for mock implementations in tests.

**Instruction for Copilot:** When writing new code, design it to be testable. When asked to write tests, use a suitable framework and create mock objects for hardware dependencies.
