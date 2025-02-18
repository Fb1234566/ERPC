#include <Arduino.h>
#include "driver/spi_slave.h"
#include "driver/spi_common.h"
#include <string.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// Define SPI slave pin configuration
#define PIN_NUM_MOSI 2   // Master Out, Slave In
#define PIN_NUM_MISO  12  // Master In, Slave Out
#define PIN_NUM_CLK  0   // Clock
#define PIN_NUM_CS    19   // Chip Select

// Transaction size in bytes
#pragma pack(push, 1)
typedef struct {
    uint8_t actionType;  // 0 = Joystick X, 1 = Joystick Y, 2 = Potenziometro, 3-6 = Pulsanti
    float value;
    float value1;
} SensorData;
#pragma pack(pop)
#define TRANSFER_SIZE 9

// Buffers for SPI communication
uint8_t txBuffer[1]; // Data to send to master
uint8_t rxBuffer[TRANSFER_SIZE]; // Data received from master


// SPI transaction structure (used to queue transactions)
spi_slave_transaction_t spiTrans;

static SensorData receivedData;

#include <WiFi.h>

const char* ssid = "";         // your wifi ssid
const char* password = "";           // your wifi password
const char* hostname = "";               // the host name of this device
const char* serverAddr = "";  // the address of the server (local ip address)
WiFiUDP udp;
void setup() {

  Serial.begin(115200);
  delay(1000);
  
  WiFi.setHostname(hostname);  // set the hostname of the device
  WiFi.begin(ssid, password);  // connect to the wifi network

  while (WiFi.status() != WL_CONNECTED) {  // wait until it connected to the wifi
    delay(100);
  }

  // Initialize the txBuffer with some example data
  for (int i = 0; i < TRANSFER_SIZE; i++) {
    txBuffer[i] = i;
  }

  // Configure the SPI bus (pins)
  spi_bus_config_t buscfg = {
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = PIN_NUM_MISO,
    .sclk_io_num = PIN_NUM_CLK,
    .quadwp_io_num = -1,
    .quadhd_io_num = -1,
    .max_transfer_sz = TRANSFER_SIZE
  };

  // Configure the SPI slave interface
  spi_slave_interface_config_t slvcfg = {
    .spics_io_num = PIN_NUM_CS,     // Chip select pin
    .flags = 0,
    .queue_size = 1,                // Transaction queue size
    .mode = 3,                      // SPI mode 0: CPOL = 0, CPHA = 0
    .post_setup_cb = NULL,          // Optional callback (after setup)
    .post_trans_cb = NULL           // Optional callback (after transaction)
  };



  // Initialize the SPI slave interface on HSPI_HOST (one of the SPI peripherals on ESP32)
  esp_err_t ret = spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, 1);
  if (ret != ESP_OK) {
    Serial.print("Failed to initialize SPI slave: ");
    Serial.println(ret);
    while (1) {
      ; // halt if initialization fails
    }
  }

  // Prepare the SPI transaction structure
  memset(&spiTrans, 0, sizeof(spiTrans));
  spiTrans.length = TRANSFER_SIZE * 8; // Transaction length in bits
  spiTrans.tx_buffer = txBuffer;       // Buffer that will be sent to master
  spiTrans.rx_buffer = rxBuffer;       // Buffer that will store data received from master

  // Queue the SPI transaction so the slave is ready for the master's request
  ret = spi_slave_queue_trans(HSPI_HOST, &spiTrans, portMAX_DELAY);
  if (ret != ESP_OK) {
    Serial.print("Failed to queue SPI transaction: ");
    Serial.println(ret);
  }
  
}
void sendData(uint8_t* d){
  udp.beginPacket(serverAddr, 5006);
  udp.write(d, 9);
  udp.endPacket();
}
void printBits(uint8_t n) {
  for (int i = 7; i >= 0; i--) {
    Serial.print((n >> i) & 1);
  }
}
void loop() {
  // Block until the SPI transaction completes

  spi_slave_transaction_t* spiTransPtr = &spiTrans;

  esp_err_t ret = spi_slave_get_trans_result(HSPI_HOST, &spiTransPtr, portMAX_DELAY);
  Serial.println(ret);
  if (ret == ESP_OK) {
    // Send data via UDP
    sendData(rxBuffer);
    
    // Reinitialize the transaction structure for the next transfer
    memset(&spiTrans, 0, sizeof(spiTrans));
    spiTrans.length = TRANSFER_SIZE * 8;  // Transaction length in bits
    spiTrans.tx_buffer = txBuffer;
    spiTrans.rx_buffer = rxBuffer;

    // Requeue the transaction so the ESP32 is ready for the next transmission from the master.
    ret = spi_slave_queue_trans(HSPI_HOST, &spiTrans, portMAX_DELAY);
    if (ret != ESP_OK) {
      Serial.print("Failed to requeue SPI transaction: ");
      Serial.println(ret);
    }
  }
}
