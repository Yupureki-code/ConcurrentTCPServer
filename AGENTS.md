# AGENTS.md

## Build

```bash
mkdir build && cd build && cmake .. && make
```

- Libraries output: `CTS/output/lib/libCTS.{a,so}` and `EchoServer/output/lib/libEchoServer.{a,so}`
- C++17 required

## Key paths

- Core library: `CTS/` (One Thread One Loop Reactor model)
- Example server: `EchoServer/` (depends on CTS)
- Tests: `tests/` (currently disabled in root CMakeLists.txt)

## Notes

- Root `CMakeLists.txt` has CTS enabled, EchoServer and tests commented out
- Build both libs: enable `add_subdirectory(EchoServer)` and `add_subdirectory(tests)` in root CMakeLists.txt
- Shared libs use VERSION 1.0.0, SOVERSION 1
