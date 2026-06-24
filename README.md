# Bitmap

A small C++17 bitmap utility library with strongly typed pixel formats, generic bitmap storage, and palette support.

## Features

- Header-first `Bitmap<TPixel>` image container with configurable pitch.
- Pixel format helpers for common layouts:
  - `Pixel565`, `Pixel1555`
  - `PixelRGB`, `PixelBGR`
  - `PixelARGB`, `PixelRGBA`, `PixelBGRA`, `PixelABGR`
- `Palette<TEntry>` with 256 entries and convenience aliases (for example `PaletteRGB`).
- Region extraction and clipped copy operations.
- CMake package export (`Bitmap::Bitmap`) for build-tree and install-tree consumption.

## Requirements

- CMake 3.23+
- C++17 compiler
- Dependency: [Rect](https://github.com/jambolo/Rect) (provides `Rect::Rect`)

## Build

```powershell
cmake -S . -B build -G Ninja
cmake --build build
```

### Build options

- `BUILD_SHARED_LIBS` (default: `OFF`)
- `Bitmap_BUILD_DOCS` (default: `OFF`) - enables Doxygen docs target when Doxygen is available
- `BUILD_TESTING` (default from CTest, usually `ON` for top-level)

## Run tests

```powershell
cmake -S . -B build -G Ninja -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Install

```powershell
cmake --install build --prefix <install-prefix>
```

This installs headers and CMake package files so consumers can link with `Bitmap::Bitmap`.

## Using from CMake

```cmake
find_package(Rect REQUIRED)
find_package(Bitmap REQUIRED)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE Bitmap::Bitmap)
```

## Example

```cpp
#include <Bitmap/Bitmap.h>
#include <Bitmap/Pixel.h>

int main()
{
	Bitmap<PixelRGBA> image(320, 240);

	PixelRGBA red;
	red.set(1.0f, 0.0f, 0.0f, 1.0f);

	for (int y = 0; y < image.height(); ++y)
	{
		for (int x = 0; x < image.width(); ++x)
		{
			image.data(x, y)[0] = red;
		}
	}

	return 0;
}
```

## Documentation

To generate API docs:

```powershell
cmake -S . -B build -G Ninja -DBitmap_BUILD_DOCS=ON
cmake --build build --target doxygen
```
