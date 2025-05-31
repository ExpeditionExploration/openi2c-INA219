# INA219 driver NodeJS-interface

Datasheet: http://www.adafruit.com/datasheets/ina219.pdf

This standalone interface provides I2C driver bindings for
[OpenI2C](https://github.com/ExpeditionExploration/openi2c/).


## About

INA219 is a current, voltage and power measurement module. This TypeScript
module wraps the [LibDriver INA219](https://github.com/libdriver/ina219) driver.

This repo contains Node bindings to that driver. The LibDriver's driver isn't
Linux specific, but these bindings are.

The bindings are written in C, and need to be compiled for them to work. There
are no pre-compiled binaries. Installing by installing OpenI2C should get you
going, but if you want this driver only, see *Building* for how to build the
binary for Node.

## Prerequisites

I2C bus needs to be enabled for your Linux, in case of Raspbian, using
`sudo raspi-config`. Also, working C toolchain is needed. In
Raspbian this can be installed by `sudo apt install build-essential`.


## Usage

Check out the [example.ts](./src/example.ts) file.


## Building

For building you need a C toolchain. I've only built this using GCC.

To build you need to install the dependencies. Write the following in the
repository root:

```bash
npm install
```

Building the wrapper module is done by using one of the npm build scripts
defined in the `package.json` file:

```bash
npm run build
```

or optionally

```bash
npm run bear-build
```

[Bear](https://github.com/rizsotto/Bear) generates `compile_commands.json`
for clangd-based autocompletions in this node-gyp project. You can install it
by `sudo apt install bear`.


## Running example

After building you can run `npx ts-node src/example.ts` to run the example file.


## Tested boards

This has been tested on:
- *Raspberry Pi 4B* using *Node v22.14.0*
- *Orange Pi CM5* using *Node v20.19.1*
