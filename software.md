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

...

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
