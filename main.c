#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>

/* Struttura per inviare dati via UART */
typedef struct {
    int actionType;  // 0 = Joystick X, 1 = Joystick Y, 2 = Potenziometro, 3-6 = Pulsanti
    float value;
} SensorData;

/* UART Configuration */
const eUSCI_UART_ConfigV1 uartConfig =
{
    EUSCI_A_UART_CLOCKSOURCE_SMCLK,  // Clock = SMCLK
    13, 0, 37,                       // Baud rate settings for 115200
    EUSCI_A_UART_NO_PARITY,
    EUSCI_A_UART_LSB_FIRST,
    EUSCI_A_UART_ONE_STOP_BIT,
    EUSCI_A_UART_MODE,
    EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,
    EUSCI_A_UART_8_BIT_LEN
};

/* ADC results buffer */
uint16_t adcResults[3];  // X, Y, Potenziometro
uint8_t xPosition = 64, yPosition = 64;  // Posizione joystick
uint16_t prevXPosition = 0xFFFF, prevYPosition = 0xFFFF, prevPotPercent = 0xFFFF;  // Per evitare invii ridondanti

/* Inizializza ADC per Joystick (X,Y) e Potenziometro */
void initializeADC() {
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN7, GPIO_TERTIARY_MODULE_FUNCTION);

    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

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

/* Inizializza hardware */
void initializeHardware() {
    WDT_A_holdTimer();

    PCM_setCoreVoltageLevel(PCM_VCORE1);
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    initializeADC();

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    UART_initModule(EUSCI_A0_BASE, &uartConfig);
    UART_enableModule(EUSCI_A0_BASE);

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN5);
    GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN5);
    Interrupt_enableInterrupt(INT_PORT4);

    // Configura LED su P1.0
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

/* Invia dati UART */
void sendUARTData(SensorData data) {
    char buffer[30];
    int i;
    sprintf(buffer, "A:%d V:%.2f\n", data.actionType, data.value);
    for (i = 0; buffer[i] != '\0'; i++) {
        UART_transmitData(EUSCI_A0_BASE, buffer[i]);
    }
    // Accende il LED per indicare un input
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

/* Interrupt Handler ADC */
void ADC14_IRQHandler(void) {
    uint64_t status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

    if (status & ADC_INT2) {
        adcResults[0] = ADC14_getResult(ADC_MEM0);
        adcResults[1] = ADC14_getResult(ADC_MEM1);
        adcResults[2] = ADC14_getResult(ADC_MEM2);

        xPosition = (adcResults[0] * 127) / 16384;
        yPosition = (adcResults[1] * 127) / 16384;
        uint16_t potPercent = (adcResults[2] * 100) / 16384;

        if (xPosition != prevXPosition) {
            SensorData joystickData = {0, xPosition};
            sendUARTData(joystickData);
            prevXPosition = xPosition;
        }

        if (yPosition != prevYPosition) {
            SensorData joystickData = {1, yPosition};
            sendUARTData(joystickData);
            prevYPosition = yPosition;
        }

        if (potPercent != prevPotPercent) {
            SensorData potData = {2, potPercent};
            sendUARTData(potData);
            prevPotPercent = potPercent;
        }
    }
}

/* Funzione principale */
int main(void) {
    initializeHardware();
    while (1) {
        PCM_gotoLPM0();
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0); // Spegne LED dopo input
    }
}
