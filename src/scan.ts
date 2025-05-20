import { openBus } from './bus';

export async function scan(busNumber: number) {
    console.log(`Scanning I2C bus ${busNumber}...`)
    const bus = openBus(busNumber);
    const addresses = await bus.scan();
    console.log(addresses);
}

const BUS = parseInt(process.env.I2C_BUS ?? '5');
scan(BUS).then(() => {
    console.log('Scan complete');
}).catch((error) => {
    console.error('Error during scan:', error);
});
