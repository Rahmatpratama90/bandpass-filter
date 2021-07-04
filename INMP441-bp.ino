

const int fs = 48000;
const int channelCount = 2;
#include <driver/i2s.h>
const i2s_port_t I2S_PORT = I2S_NUM_0; 
#include "Arduino.h"
#include "driver/gpio.h"
#include <YummyDSP.h>
#include <AudioDriver.h>
AudioDriver i2sCodec;
YummyDSP dsp;
FilterNode lp;
FilterNode hp;
FilterNode bp; // a bandpass pass filter
WaveShaperNode sat;
DelayNode dly;
WaveShaperNode sat; // some saturation
#include <driver/i2s.h>

void setup() {
  dsp.addNode(&bp);
//dsp.add(&sat);

  Serial.begin(115200);
  Serial.println("Configuring I2S...");
  esp_err_t err;
  // The I2S config 
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // Receive mode
      .sample_rate = 16000,                         // 16KHz
      // could only get it to work with 32bits/24bits 
.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, 
      // leave L/R unconnected when using Left channel
.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, 
// Philips standard | MSB first standard
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S| I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,     // Interrupt level 1
      .dma_buf_count = 4,                           // number of buffers
      .dma_buf_len = 8                         // 8 samples per buffer (minimum)
  };

  // The pin config 
  const i2s_pin_config_t pin_config = {
      .bck_io_num = 15,   // BCKL SCK
      .ws_io_num = 18,    // LRCL WS
      .data_out_num = -1, // not used (only for speakers)
      .data_in_num = 19   // DOUT SD
  };

  // Configuring the I2S driver and pins.
  // This function must be called before any I2S driver read/write operations.
  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Failed installing driver: %d\n", err);
    while (true);
  }
  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("Failed setting pin: %d\n", err);
    while (true);
  }
  Serial.println("I2S driver installed.");
}
void loop() {
  i2sCodec.readBlock();
float sample =0;
  for (int ch = 0; ch < channelCount; ch++) {

      sample = i2sCodec.readSample(i, ch);
      
sample = dsp.process(sample);

  // Read a single sample and log it for the Serial Plotter.
  //int32_t sample = 0;
 // int sample2= sample+1;
  int bytes_read = i2s_pop_sample(I2S_PORT, (char *)&sample, portMAX_DELAY); // no timeout
  if (bytes_read > 0) {
    Serial.println("sample :");
    Serial.println(sample);
  } 
  delay(100);
}


}
