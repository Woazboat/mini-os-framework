# mini-os-framework
Minimal framework for x86 OS development

Set up your build directory:
`mkdir build && cd build && cmake ..`

Re-run `cmake .` in your build directory when adding/removing/renaming source files


* `make` Compile the kernel
* `make diskimage` Create a bootable disk image
* `make qemu` Run your OS in QEMU
