# SQLite C sample

A minimal C program demonstrating the SQLite C API:

- create / open a database file
- create a table
- insert rows (with prepared statements + bound parameters)
- update rows
- search (`SELECT ... WHERE name LIKE ?`)
- delete rows
- drop table
- close and remove the DB file

## Build (Linux / macOS)

```bash
sudo apt-get install -y libsqlite3-dev cmake build-essential   # Ubuntu/Debian
cmake -S . -B build
cmake --build build
./build/sqlite_sample
```

## Build (Windows)

With [vcpkg](https://vcpkg.io):

```powershell
vcpkg install sqlite3
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=<vcpkg-root>/scripts/buildsystems/vcpkg.cmake
cmake --build build
.\build\Debug\sqlite_sample.exe
```

## Build & run inside Docker (Ubuntu)

From this directory:

```bash
docker run --rm -v "$PWD":/src -w /src ubuntu:22.04 bash -c "
  apt-get update &&
  apt-get install -y --no-install-recommends cmake build-essential libsqlite3-dev &&
  cmake -S . -B build &&
  cmake --build build -j &&
  ./build/sqlite_sample
"
```

On PowerShell, replace `$PWD` with `${PWD}`.
