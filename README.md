# INA219 driver

Datasheet: http://www.adafruit.com/datasheets/ina219.pdf


## About

INA219 is a current, voltage and power measurement module.


## Usage

See the `./src/example.ts`.


## Running tests

To install dependencies run `npm i`. Tests are run with command
`OPENI2C_MOCKED=true I2C_BUS=1 I2C_ADDRESS=addr npm test`. Bus number defaults
to bus `5` and address `0x40`. Leave the `OPENI2C_MOCKED` environment variable
out if you wish to test with real device. You can also scan devices on a bus
with `I2C_BUS=1 npm run scan`.

