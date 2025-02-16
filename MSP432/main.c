#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <stdio.h>

#include <stdint.h>

#include <stdbool.h>

/* Statics */
static volatile uint8_t RXData = 0;

/* SPI Master Configuration Parameter */
const eUSCI_SPI_MasterConfig spiMasterConfig =
{
        EUSCI_B_SPI_CLOCKSOURCE_SMCLK,             // SMCLK Clock Source
        3000000,                                   // SMCLK = DCO = 3MHZ
        500000,                                    // SPICLK = 500khz
        EUSCI_B_SPI_MSB_FIRST,                     // MSB First
        EUSCI_B_SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURED_ON_NEXT,    // Phase
        EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_HIGH, // High polarity
        EUSCI_SPI_4PIN_UCxSTE_ACTIVE_LOW                           // 4Wire SPI Mode

};


/* Struttura per inviare dati via UART */
#pragma pack(push, 1)
typedef struct {
    uint8_t actionType;  // 0 = Joystick X, 1 = Joystick Y, 2 = Potenziometro, 3-6 = Pulsanti
    float value;
    float value1;
} SensorData;
#pragma pack(pop)

/* ADC results buffer */
SensorData joystickData;
SensorData potData;
uint16_t adcResults[3];  // X, Y, Potenziometro
uint8_t xPosition = 64, yPosition = 64, potPercent=0;  // Posizione joystick
uint16_t prevXPosition = 0xFFFF, prevYPosition = 0xFFFF, prevPotPercent = 0xFFFF;  // Per evitare invii ridondanti
SensorData dataQueue[10];
int queueIdx = 0;

/* Inizializza ADC per Joystick (X,Y) e Potenziometro */
void initializeADC() {
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN7, GPIO_TERTIARY_MODULE_FUNCTION);

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

void initializeSPI(){
    /* Selecting P1.5 P1.6 and P1.7 in SPI mode */
        GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P5, GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P1,
                                                      GPIO_PIN5 | GPIO_PIN6,
                                                      GPIO_PRIMARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
                                                     GPIO_PIN7,
                                                     GPIO_PRIMARY_MODULE_FUNCTION);

        /* Configuring SPI in 4wire master mode */
        GPIO_setOutputHighOnPin(5, 2);
        SPI_initMaster(EUSCI_B0_BASE, &spiMasterConfig);
        SPI_selectFourPinFunctionality(EUSCI_B0_BASE, EUSCI_SPI_ENABLE_SIGNAL_FOR_4WIRE_SLAVE);

        /* Enable SPI module */
        SPI_enableModule(EUSCI_B0_BASE);

        /* Enabling interrupts */
        SPI_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_RECEIVE_INTERRUPT);
        //SPI_enableInterrupt(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT);
        Interrupt_enableInterrupt(INT_EUSCIB0);
        Interrupt_enableSleepOnIsrExit();
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
    initializeSPI();
    initializeADC();

    //if (status & GPIO_PIN2) sendSPIData(&(SensorData){4, (float)0.0, (float)0.0});
    //if (status & GPIO_PIN3) sendSPIData(&(SensorData){5, (float)0.0, (float)0.0});
    //if (status & GPIO_PIN5) sendSPIData(&(SensorData){6, (float)0.0, (float)0.0});
    // Pushbutton joystick
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN1);
    Interrupt_enableInterrupt(INT_PORT4);

    // Pushbutton 1 boosterpack
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P3, GPIO_PIN5);
    Interrupt_enableInterrupt(INT_PORT3);

    // Pushbutton 2 boosterpack
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN1);
    Interrupt_enableInterrupt(INT_PORT5);


    // Configura LED su P1.0
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

/* Invia dati SPI */
void sendSPIData(SensorData data) {
    while (!(SPI_getInterruptStatus(EUSCI_B0_BASE, EUSCI_B_SPI_TRANSMIT_INTERRUPT)));
    GPIO_setOutputLowOnPin(5, 2);
    uint8_t buffer[sizeof(SensorData)];
    memcpy(buffer, &data, sizeof(SensorData));

    int i;
    for(i=0; i<sizeof(SensorData); i++){
     SPI_transmitData(EUSCI_B0_BASE, buffer[i]);
    }

    GPIO_setOutputHighOnPin(5, 2);
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

        xPosition = (((uint16_t)((adcResults[0]+420) / 820) * 820 ) * 127) / 16384;

        yPosition = (((uint16_t)((adcResults[1]+420) / 820) * 820 ) * 127) / 16384;
        potPercent = (((uint16_t)((adcResults[2]+420) / 820) * 820 )* 100) / 16384;

        if (xPosition != prevXPosition || yPosition != prevYPosition) {
            //joystickData = (SensorData){1, (float)xPosition, (float)yPosition};
            sendSPIData((SensorData){1, (float)xPosition, (float)yPosition});
            prevXPosition = xPosition;
            prevYPosition = yPosition;
        }

        if (potPercent != prevPotPercent) {
            //potData = (SensorData){2, (float)potPercent, (float)0.0};
            sendSPIData((SensorData){2, (float)potPercent, (float)0.0});
            prevPotPercent = potPercent;
        }
    }
}

/* Interrupt Handler Pulsanti */
void PORT4_IRQHandler(void) {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P4);
    GPIO_clearInterruptFlag(GPIO_PORT_P4, status);

    if (status & GPIO_PIN1) sendSPIData((SensorData){3, (float)0.0, (float)0.0});
}
/* Interrupt Handler Pulsanti */
void PORT3_IRQHandler(void) {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);

    if (status & GPIO_PIN5) sendSPIData((SensorData){4, (float)0.0, (float)0.0});
}
/* Interrupt Handler Pulsanti */
void PORT5_IRQHandler(void) {
    uint32_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);

    if (status & GPIO_PIN1) sendSPIData((SensorData){5, (float)0.0, (float)0.0});
}

/* Funzione principale */
int main(void) {
    initializeHardware();
    while (1) {
        PCM_gotoLPM0();
        GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0); // Spegne LED dopo input
    }
}

void EUSCIB0_IRQHandler(void){

}

