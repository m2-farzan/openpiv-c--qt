OpenPIV (c++)
=============

An implementation of a PIV analysis engine in C++ using as few dependencies as possible;
the implementation requires a c++17 compliant compiler.

Build
-----

Building uses cmake; to build (linux example):

* mkdir build
* cd build
* cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake ..
* make

To run tests:

* make test

Dependencies
------------

* c++17 compiler e.g. clang++-5.0, gcc7
* [vcpkg](https://github.com/Microsoft/vcpkg)
  * catch2: unit test framework: vcpkg install catch2
  * libtiff: TIFF IO support: vcpkg install tiff
  * spdlog: logging framework (not yet required): vcpkg install spdlog
  * benchmark: used to run performance benchmarks: vcpkg install benchmark

TODO
----

* core
  * [ ] logging
  * [x] load/save PNM files
    * [x] save double image
  * [ ] iostream ops for ImageLoaders
  * [ ] shared memory -> Image allocator support
  * utilities
    * [x] split RGB to 4xgreyscale
    * [x] join 4xgreyscale to RGB
    * [x] split complex to planes
    * [x] create complex from planes
  * [x] add ImageInterface data iterators
  * [x] remove data() method from ImageInterface, ImageView
  * [x] allow ImageView to be non-const
* algorithms
  * [x] transpose
  * [x] normalize
  * FFT
    * [x] spectral correlation
    * [ ] optimize
    * [ ] apply kernel in Fourier space
    * [ ] use SIMD
  * [ ] direct correlation
  * [x] peak detection
  * [ ] peak fitting
* image processing
  * [ ] low/high pass filter
* processing framework
  * [ ] grid generator
  * [ ] processing
  * [ ] marking
* data output
  * [ ] ASCII/CSV
  * [ ] gnuplot/pyplot?
  * [ ] tecplot
* GUI
  * [ ] Qt?
  * [ ] websockets/HTML5?
  * [ ] webassembly + JS/HTML5?
