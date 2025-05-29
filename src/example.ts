
import { ADCMode, BusVoltageRange, I2CAddress, ina219, PGAGain } from ".";
import { sleep } from "./utils";

async function main() {
    try {
        // Initialize the INA219 sensor
        await ina219.basicInit(
            I2CAddress.ADDRESS_0,
            0.5,
            BusVoltageRange.VBUS_RANGE_32V,
            ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            PGAGain.GAIN_1_DIV_8
        )
        while (true) {
            const shuntVoltage = await ina219.getShuntVoltage();
            const busVoltage = await ina219.getBusVoltage();
            const current = await ina219.getCurrent();
            console.log(`Shunt Voltage: ${shuntVoltage} mV`);
            console.log(`Bus Voltage: ${busVoltage} mV`);
            console.log(`Current: ${current} mA`);
            console.log(`Power: ${ina219.getPower()} mW`);
            await sleep(1000); // Wait for 1 second before the next reading
            console.log("-------------------------------");
        }

    } catch (error) {
        console.error("Error initializing INA219:", error);
    }
}

main().then(() => {
    console.log("INA219 initialized successfully.");
}).catch((error) => {
    console.error("Failed to initialize INA219:", error);
});
