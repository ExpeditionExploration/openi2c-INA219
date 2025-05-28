
import { ADCMode, BusVoltageRange, I2CAddress, ina219, PGAGain } from ".";

async function main() {
    try {
        // Initialize the INA219 sensor
        const sensor = await ina219.basicInit(
            I2CAddress.ADDRESS_0,
            0.5,
            BusVoltageRange.VBUS_RANGE_32V,
            ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            ADCMode.ADC_MODE_12_BIT_128_SAMPLES,
            PGAGain.GAIN_1_DIV_8
        )
    } catch (error) {
        console.error("Error initializing INA219:", error);
    }
}

main().then(() => {
    console.log("INA219 initialized successfully.");
}).catch((error) => {
    console.error("Failed to initialize INA219:", error);
});
