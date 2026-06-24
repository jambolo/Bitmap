# Copilot Instructions for Bitmap

## Project overview
- Language: C++17
- Build system: CMake (minimum 3.23)
- Typical generator/toolchain: Ninja + MSVC
- Library target: `Bitmap` (alias `Bitmap::Bitmap`)
- External dependency: `Rect::Rect`
- Tests: GoogleTest in `test/`

## Repository structure
- Public headers: `include/Bitmap/`
- Library source: root-level `.cpp` files (for example `Pixel.cpp`)
- Build config: `CMakeLists.txt`, `cmake/`
- Tests: `test/test-*.cpp`
- Docs config: `Doxyfile.in`

## Coding guidelines
- Preserve existing public API names and include paths under `Bitmap/`.
- Keep changes minimal and scoped to the request.
- Prefer standard library features over adding new dependencies.
- Follow existing style in each file (indentation, brace style, naming).
- Do not add comments unless necessary and consistent with surrounding code.
- Avoid C++ extensions; keep code compatible with C++17 and `CXX_EXTENSIONS OFF`.

## CMake guidelines
- Keep target-based CMake patterns (`target_sources`, `target_link_libraries`, etc.).
- Do not hardcode platform-specific compiler flags beyond existing conventions.
- Preserve install/export behavior for `Bitmap::Bitmap` package consumption.
- Respect options already used by this project:
  - `BUILD_SHARED_LIBS`
  - `Bitmap_BUILD_DOCS`
  - `BUILD_TESTING`

## Testing expectations
- For code changes, add or update focused GoogleTest coverage in `test/` when appropriate.
- Keep tests deterministic and platform-neutral.
- Prefer small unit tests over broad integration-style changes.

## Documentation expectations
- When behavior or usage changes, update `README.md` and/or Doxygen-facing comments in headers.
- Keep examples buildable and aligned with current API.
