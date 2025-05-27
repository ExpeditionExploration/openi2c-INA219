# INA219 driver

Datasheet: http://www.adafruit.com/datasheets/ina219.pdf


## About

INA219 is a current, voltage and power measurement module. This TypeScript
module wraps the [LibDriver INA219](https://github.com/libdriver/ina219) driver.


## Usage

tbd


## Running tests

tbd


## Development


### Building

Building the wrapper module is done by using one of the npm build scripts
defined in the `package.json` file:

```bash
npm run bear-build
```

or
```bash
npm run build
```

[Bear](https://github.com/rizsotto/Bear) is a tool which generates the
`compile_commands.json`-file. I did not get clangd completions to work without
it in this node-gyp project.
