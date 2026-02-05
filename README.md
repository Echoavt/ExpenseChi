# Expense Tracker (Qt 6)

A production-lean MVP desktop expense tracker built with **Qt 6**, **Qt Quick Controls 2 (Material)**, a **C++ backend**, and **SQLite** persistence. It supports light/dark theme persistence, monthly navigation, summary dashboards, filters, and full CRUD with undo.

## Features
- Material-inspired UI with cards, FAB, dialogs, and snackbar.
- Dark/Light theme toggle with persistence (default: Dark).
- Monthly navigation and summary cards (Income, Expenses, Net).
- Filterable transactions list with full-text search.
- Add/Edit/Delete transactions with undo.
- SQLite storage in the user AppData location with simple migrations.

## Build in Qt Creator (Windows/macOS/Linux)

1. **Install Qt 6** with the modules:
   - Qt Quick
   - Qt Quick Controls 2
   - Qt SQL

2. **Open the project**
   - Launch **Qt Creator** → **Open Project** → select `CMakeLists.txt`.

3. **Configure Kit**
   - Select your desired kit (MinGW/MSVC on Windows, or default kit on macOS/Linux).

4. **Build & Run**
   - Click **Build** → **Run**.

The database is stored under your system's standard app data directory (e.g., `AppData/Roaming` on Windows, `~/.local/share` on Linux, and `~/Library/Application Support` on macOS).

## Release Build Notes
- Use **Release** configuration in Qt Creator.
- Ensure `Qt6_DIR` is set in your environment if needed.
- The resulting binary will be in the build output directory (`build-*/` by default).

## Windows Deployment Notes
To run on another Windows machine without a Qt installation:

1. Build in **Release** mode.
2. Use `windeployqt` on the compiled executable:
   ```
   windeployqt --release path\to\ExpenseTracker.exe
   ```
3. Ensure the SQLite driver is deployed (`qsqlite.dll`). `windeployqt` usually handles this automatically, but verify that the `sqldrivers` folder includes it.
4. Copy the output folder to the target machine and run the executable.

## License
MIT (add your preferred license as needed).
