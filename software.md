# Software

## Installation: OSS CAD Suite

Download from their releases page on GitHub:

I picked up the "2025-11-03" version:

https://github.com/YosysHQ/oss-cad-suite-build/releases/download/2025-11-03/oss-cad-suite-linux-x64-20251103.tgz

Download and extract:

``` bash
wget $LINK
tar xzf $ZIP
```

Activate Environment:

``` bash
source ./oss-cad-suite/environment
```

## Verilator

Verilator converts a `verilog` file and generates `c` files, as well as a
Makefile. There are a few ways to use verilator ... but how I want to use it is
to include Verilated C libraries into our test executable, and run all the tests
for the hardware at once. 

References / Links:

- [Verilator CLI Args](https://verilator.org/guide/latest/exe_verilator.html)
- [Notes on Verilating](https://github.com/verilator/verilator/blob/master/docs/guide/verilating.rst)

Building with Verilator is annoying. It is easiest to use the `--exe` arg with
the `--build` option to actually get the thing to build. And `--cc` to generate
the C files. When building with your own main file, you can list the C and V
files to include directly:

``` bash
verilator fifo.v main.c --build --exe --cc
```

You also have to include the `CFLAGS` and `LDFLAGS` if you need any of those.
These flags, if they include paths, should probably be "absolute" instead of "relative":

``` bash
verilator fifo.v main.c --build --exe -CFLAGS "..." -LDFLAGS "..."
```

The only V file that is needed is the Top V file. To add paths to the Verilog
search path, use `-y`:

``` bash
verilator ... -y /usr/local/share/verilog/
```

What's neat about `-y` is that this directory will be searched for C files as
well as verilog files? Kinda Neat?

To enable VCD traces, add the `--trace` flag. To add C++ coroutine support for
timing and such, add `--timing`.

## SBY

Create a `.sby` file, which tells SBY what verilog files to use:

```
[options]
mode bmc
depth 20

[engines]
smtbmc

[script]
read -formal fifo.v
prep -top fifo
opt_merge -share_all

[files]
fifo.v
```

Normal Usage:

``` bash
sby fifo.sby
```

Overwrite the current output directory:

``` bash
sby fifo.sby -f
```

Do work in a temp directory, and generate errors if there are any:

``` bash
sby fifo.sby -t -E
```
