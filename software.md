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

In short, we can generate a number of C files and a Makefile using the `--cc`
option:

``` bash
verilator --cc fifo.v
```

The default output directory is `obj_dir`. We can change this using the `--Mdir`
option:

``` bash
verilator --cc fifo.v --Mdir obj_dir_fifo
```

Once the C source is generated, we can use Make to build it:

``` bash
cd obj_dir_fifo/
make -f Vfifo.mk
```

This builds two libraries: 

- `libverilated.a` 
- `libVfifo.a`

I believe that `libverilated.a` is the same across modules? 

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
