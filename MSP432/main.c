#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

/* Static variables */
static volatile uint8_t receivedData = 0;  // Stores received data via SPI

/* SPI Master Configuration */
const eUSCI_SPI_MasterConfig spiMasterConfig =
{
    EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             // Clock source: SMCLK
    3000000,                                   // SMCLK = DCO = 3MHz
    500000,                                    // SPI Clock = 500kHz
    EUSCI_B_SPI_MSB_FIRST,                     // MSB First transmission
    EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,  // Clock phase setting
    EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH, // Clock polarity: High
    EUSCI_SPI_4PIN_UCxSTE_ACTIVE_LOW           // 4-wire SPI mode (Slave Select Active Low)
};

/* Data structure to send via SPI (pack to avoid padding) */
#pragma pack(push, 1)
typedef struct {
    uint8_t actionType;  // Action type (e.g., 0 = Joystick X, 1 = Joystick Y, 2 = Potentiometer)
    float value;         // First data value (e.g., joystick X or Y position, potentiometer value)
    float value1;        // Second data value (optional, used for extra data if needed)
} SensorData;
#pragma pack(pop)

/* ADC results buffer and state variables */
SensorData joystickData;
SensorData potentiometerData;
uint16_t adcResults[3];  // Stores ADC results for X, Y, Potentiometer
uint8_t joystickX = 64, joystickY = 64, potentiometerPercent = 0;  // Joystick positions and potentiometer percentage
uint16_t previousX = 0xFFFF, previousY = 0xFFFF, previousPot = 0xFFFF;  // To prevent redundant sending of data


/* Initialize ADC for Joystick (X, Y) and Potentiometer */
void initializeADC() {
    // Configure ADC pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN7, GPIO_TERTIARY_MODULE_FUNCTION);

    // Initialize ADC14 module and configure settings
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_ACLK, ADC_PREDIVIDER_32, ADC_DIVIDER_1, 0);
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM2, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A15, ADC_NONDIFFERENTIAL_INPUTS);
    ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);
    ADC14_configureConversionMemory(ADC_MEM2, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A6, ADC_NONDIFFERENTIAL_INPUTS);

    ADC14_enableInterrupt(ADC_INT2);
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();
}

/* Initialize SPI for communication with external devices */
void initializeSPI() {
    // Configure SPI pins (P1.5, P1.6, P1.7 for SPI communication)
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1, GPIO_PIN5 | GPIO_PIN6, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);

    // Set up SPI in 4-wire master mode
    GPIO_setOutputHighOnPin(5, 2);
    SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfig);
    SPI_selectFourPinFunctionality(EUSCI_B0_BASE, EUSCI_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE);

    // Enable SPI module and configure interrupts
    SPI_enableModule(EUSCI_B0_BASE);
    Interrupt_enableInterrupt(INT_EUSCIB0);
    Interrupt_enableSleepOnIsrExit();
}

/* Initialize hardware components and peripherals */
void initializeHardware() {
    WDT_A_holdTimer();  // Stop Watchdog Timer

    // Set voltage core level and Flash wait state
    PCM_setCoreVoltageLevel(PCM_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    // Set up DCO clock source (48 MHz)
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // Initialize SPI and ADC
    initializeSPI();
    initializeADC();

    // Set up pushbutton interrupts
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN1);
    Interrupt_enableInterrupt(INT_PORT4);

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN5);
    Interrupt_enableInterrupt(INT_PORT3);

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN1);
    Interrupt_enableInterrupt(INT_PORT5);
}

/* Send data via SPI */
void sendSPIData(SensorData data) {
    while (!(SPI_getInterruptStatus(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT)));  // Wait for previous transmission to finish
    GPIO_setOutputLowOnPin(5, 2);  // Select SPI slave

    uint8_t buffer[sizeof(SensorData)];
    memcpy(buffer, &data, sizeof(SensorData));

    // Send each byte of the data structure
    for (int i = 0; i < sizeof(SensorData); i++) {
        SPI_transmitData(EUSCI_B0_BASE, buffer[i]);
    }

    GPIO_setOutputHighOnPin(5, 2);  // Deselect SPI slave
}

/* ADC Interrupt Handler */
void ADC14_IRQHandler(void) {
    uint64_t status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    if (status & ADC_INT2) {
        adcResults[0] = ADC14_getResult(ADC_MEM0);  // X-axis
        adcResults[1] = ADC14_getResult(ADC_MEM1);  // Y-axis
        adcResults[2] = ADC14_getResult(ADC_MEM2);  // Potentiometer

        // Calculate joystick positions (scaled to 127 range)
        joystickX = (((uint16_t)((adcResults[0] + 420) / 840) * 840 ) * 127) / 16384;
        joystickY = (((uint16_t)((adcResults[1] + 420) / 840) * 840 ) * 127) / 16384;

        // Calculate potentiometer percentage
        potentiometerPercent = (((uint16_t)((adcResults[2] + 420) / 840) * 840 ) * 100) / 16384;

        // Send updated joystick positions if they have changed
        if (joystickX != previousX || joystickY != previousY) {
            sendSPIData((SensorData){1, (float)joystickX, (float)joystickY});
            previousX = joystickX;
            previousY = joystickY;
        }

        // Send updated potentiometer value if it has changed
        if (potentiometerPercent != previousPot) {
            sendSPIData((SensorData){2, (float)potentiometerPercent, 0.0f});
            previousPot = potentiometerPercent;
        }
    }
}

/* Button Interrupt Handlers */
void PORT4_IRQHandler(void) {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P4);
    GPIO_clearInterruptFlag(GPIO_PORT_P4, status);
    if (status & GPIO_PIN1) sendSPIData((SensorData){3, 0.0f, 0.0f});
}

void PORT3_IRQHandler(void) {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
    if (status & GPIO_PIN5) sendSPIData((SensorData){4, 0.0f, 0.0f});
}

void PORT5_IRQHandler(void) {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
    if (status & GPIO_PIN1) sendSPIData((SensorData){5, 0.0f, 0.0f});
}

/* Main Function */
int main(void) {
    initializeHardware();
    while (1) {
        PCM_gotoLPM0();  // Go into low power mode
    }
}
