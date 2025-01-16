#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/devices/msp432p4xx/driverlib/uart.h>
#include <ti/grlib/grlib.h>
#include <stdio.h>

/* UART Configuration Structure */
const eUSCI_UART_ConfigV1 uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        13,                                      // BRDIV = 13
        0,                                       // UCxBRF = 0
        37,                                      // UCxBRS = 37
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_MSB_FIRST,                  // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
        EUSCI_A_UART_8_BIT_LEN                  // 8 bit data length
};

/* ADC results buffer */
static uint16_t adcResults[2];

/* Initialize ADC module */
void initializeADC() {
    // Configure Pin 6.0 and 4.4 as ADC inputs
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

    // Initialize ADC (ADCOSC/64/8)
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

    // Configure ADC Memory (ADC_MEM0 - ADC_MEM1 for channels A15 and A9) with repeat mode
    // and internal 2.5v reference
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);
    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A15, ADC_NONDIFFERENTIAL_INPUTS);
    ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);

    // Enable interrupt for end of sequence on channel 1
    ADC14_enableInterrupt(ADC_INT1);

    // Enable global interrupts
    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    // Set up sample timer for automatic iteration
    ADC14_enableSampleTimer(ADC_AUTOMATIC_ITERATION);

    // Start ADC conversions
    ADC14_enableConversion();
    ADC14_toggleConversionTrigger();
}

/* Convert integer to string in specified base */
char* intToStr(int value, char* result, int base) {
    // Ensure base is valid
    if (base < 2 || base > 36) {
        *result = '\0';
        return result;
    }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while (value);

    // Add negative sign if needed
    if (tmp_value < 0) {
        *ptr++ = '-';
    }

    *ptr-- = '\0';

    // Reverse the string
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }

    return result;
}

/* Initialize hardware */
void initializeHardware() {
    // Halt Watchdog Timer and disable master interrupts
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    // Set core voltage level to VCORE1
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    // Set flash wait states for Flash bank 0 and 1
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    // Initialize Clock System
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    // Initialize ADC
    initializeADC();

    // Configure UART pins and initialize UART module
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1, GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    UART_initModule(EUSCI_A0_BASE, &uartConfig);
    UART_enableModule(EUSCI_A0_BASE);

}

/* Main function */
int main(void) {
    // Initialize hardware
    initializeHardware();

    // Enter low-power mode
    while (1) {
        PCM_gotoLPM0();
    }
}

/* ADC Interrupt Handler */
void ADC14_IRQHandler(void) {
    uint64_t interruptStatus;
    uint64_t xPosition = 0;
    uint64_t yPosition = 0;
    char buffer[20];
    int i;

    // Get and clear interrupt status
    interruptStatus = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(interruptStatus);

    // Check if ADC_MEM1 conversion completed
    if (interruptStatus & ADC_INT1) {
        // Store ADC conversion results
        adcResults[0] = ADC14_getResult(ADC_MEM0);
        adcResults[1] = ADC14_getResult(ADC_MEM1);

        // Update positions based on ADC results
        while (adcResults[0] != 0 || adcResults[1] != 0) {
            adcResults[0] = ADC14_getResult(ADC_MEM0);
            adcResults[1] = ADC14_getResult(ADC_MEM1);

            // Update xPosition based on ADC_MEM0 result
            if (adcResults[0] / 100 > 100) {
                xPosition += 1;
            }
            if (adcResults[0] / 100 < 60) {
                xPosition -= 1;
            }
            xPosition %= 128;

            // Update yPosition based on ADC_MEM1 result
            if (adcResults[1] / 100 > 100) {
                yPosition -= 1;
            }
            if (adcResults[1] / 100 < 60) {
                yPosition += 1;
            }
            yPosition %= 128;
        }

        int buttonPressed = 0;
        if (!(P4IN & GPIO_PIN1))
            buttonPressed = 1;

        // Format and send positions via UART
        sprintf(buffer, "X:%llu Y:%llu B:%d\n", xPosition, yPosition, buttonPressed);
        for (i = 0; buffer[i] != '\0'; i++) {
            UART_transmitData(EUSCI_A0_BASE, buffer[i]);
        }

        // Small delay loop
        for (i = 0; i < 10; i++) {}
    }
}
