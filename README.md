# Teaching Management System

Qt6 + C++17 desktop application — local SQLite database + MVC architecture + data visualization + user authentication.

## What it solves

Educational institutions need a lightweight, zero-config desktop management system to handle student records, course schedules, payment records, and an honor wall. No server required — one executable to run.

## Tech stack

Qt 6.8 · C++17 · SQLite 3 · CMake · Qt Charts · QSS styling

## Features

- **Student management**: CRUD + photo upload (stored in DB) + paginated tables
- **Course scheduling**: weekly timetable view, custom academic year/term
- **Finance management**: payment records + pie chart (by type) + line chart (trend analysis)
- **Honor wall**: grid display + click-to-preview + images stored in DB
- **User authentication**: SHA-256 password hashing + remember me + auto-login
- **System settings**: database path, theme switching, password change

## Usage

```bash
# Qt Creator: open CMakeLists.txt → build → run

# Command line
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
./EdycationSystem
```

On first login, update the default account in the `users` table before use.
