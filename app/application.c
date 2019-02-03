#include <application.h>
#include "SPIRIT_Config.h"
#include <bc_timer.h>

// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

/*  Radio configuration parameters  */
#define RF_433_XTAL_OFFSET_PPM             0
#define RF_433_BASE_FREQUENCY              433842000
#define RF_433_CHANNEL_SPACE               20e3
#define RF_433_CHANNEL_NUMBER              0
#define RF_433_MODULATION_SELECT           ASK_OOK
#define RF_433_DATARATE                    1430 //    1 s / 700 us = 1430
#define RF_433_FREQ_DEVIATION              20e3
#define RF_433_BANDWIDTH                   100E3

static SRadioInit xRadioInit = {
  RF_433_XTAL_OFFSET_PPM,
  RF_433_BASE_FREQUENCY,
  RF_433_CHANNEL_SPACE,
  RF_433_CHANNEL_NUMBER,
  RF_433_MODULATION_SELECT,
  RF_433_DATARATE,
  RF_433_FREQ_DEVIATION,
  RF_433_BANDWIDTH
};

uint8_t rf_plug[] = {
    0x96, 0x4b, 0x6d, 0xb6, 0xdb, 0x6c, 0xb6, 0xc8
};

void send_rf_command(uint8_t *data, uint8_t len)
{
    SpiritCmdStrobeFlushTxFifo();

    SpiritDirectRfSetTxMode(DIRECT_TX_FIFO_MODE);

    SpiritPktMbusSetPayloadLength(len);
    SpiritSpiWriteLinearFifo(len, data);

    SpiritCmdStrobeTx();
}

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        bc_led_pulse(&led, 100);

        for( int i = 0; i < 4; i++)
        {
            send_rf_command(rf_plug, sizeof(rf_plug));

            HAL_Delay(100);
        }
    }

    // Logging in action
    bc_log_info("Button event handler - event: %i", event);
}


void application_init(void)
{
    // Initialize logging
    bc_log_init(BC_LOG_LEVEL_DUMP, BC_LOG_TIMESTAMP_ABS);

    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);
    bc_led_pulse(&led, 2000);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    SpiritRadioSetXtalFrequency(50000000);
    SpiritSpiInit();
    SpiritEnterShutdown();
    SpiritExitShutdown();
    SpiritManagementWaExtraCurrent();

    /* Spirit IRQ config */
    //SpiritGpioInit(&xGpioIRQ);

    /* Spirit Radio config */
    if (SpiritRadioInit(&xRadioInit) != 0)
    {
        bc_spirit1_hal_shutdown_high();
        return;
    }
}





























/*
            uint8_t data_stop[] = {
            0x0f, 0xff, 0xff, 0xf8, // preambule
            0xE8, 0xE8, 0x88, 0xee,
            0x88, 0xe8, 0x8e, 0xee,
            0xee, 0x88, 0x8e, 0xe8,  //64ms od zacatku
            0xe8, 0x88, 0x88, 0x8e,  //80.5ms od zacatku
            0x88, 0x88, 0x88, 0x8e,  // 97ms
            0x88, 0x88, 0x88, 0x88, //113.5
            0x88, 0xe8, 0x88, 0xee, //130ms
            0xe8, 0x8e, 0x88, 0xe8, // 146.5
            0xe0
            };

            uint8_t data_pre_down[] = {
            0x0f, 0xff, 0xff, 0xf8, // preambule
            0xE8, 0xE8, 0x88, 0xee,
            0x88, 0xe8, 0x8e, 0xee,
            0xee, 0x88, 0x8e, 0xe8,  //64ms od zacatku
            0xe8, 0x88, 0x88, 0x8e,  //80.5ms od zacatku
            0x88, 0x88, 0x88, 0x8e,  // 97ms
            0x88, 0x88, 0x88, 0x88, //113.5
            0x8e, 0x88, 0x88, 0xee, //130ms
            0xe8, 0xee, 0x88, 0xe8, // 146.5
            0xe0
            };

            uint8_t data_down[] = {
            0x0f, 0xff, 0xff, 0xf8, // preambule, začátek zkrácený o bit
            0xE8, 0xE8, 0x88, 0xee,
            0x88, 0xe8, 0x8e, 0xee,
            0xee, 0x88, 0x8e, 0xe8,  //64ms od zacatku
            0xe8, 0x88, 0x88, 0x8e,  //80.5ms od zacatku
            0x88, 0x88, 0x88, 0x8e,  // 97ms
            0x88, 0x88, 0x88, 0x88, //113.5
            0x88, 0xe8, 0x8e, 0x88, //130ms
            0xe8, 0x8e, 0x88, 0xee, // 146.5
            0xe0
            };


            uint8_t garage_door[] = {
                0xb2, 0xcb, 0x2c, 0xb2, 0xc8
            };

            uint8_t garage_door_fake[] = {
                0xb2, 0xcb, 0x2c, 0x55, 0x55
            };



#define RF_433_POWER_DBM                   11.6 // max is 11.6

#define RF_433_RSSI_THRESHOLD              0x20

//  Packet configuration parameters
#define RF_433_PREAMBLE_LENGTH             PKT_PREAMBLE_LENGTH_01BYTE
#define RF_433_SYNC_LENGTH                 PKT_SYNC_LENGTH_4BYTES
#define RF_433_SYNC_WORD                   0x00
#define RF_433_LENGTH_TYPE                 PKT_LENGTH_VAR
#define RF_433_LENGTH_WIDTH                8
#define RF_433_CRC_MODE                    PKT_NO_CRC
#define RF_433_CONTROL_LENGTH              PKT_CONTROL_LENGTH_0BYTES
#define RF_433_EN_ADDRESS                  S_DISABLE
#define RF_433_EN_FEC                      S_DISABLE
#define RF_433_EN_WHITENING                S_DISABLE

//  Addresses configuration parameters
#define RF_433_EN_FILT_MY_ADDRESS          S_DISABLE
#define RF_433_MY_ADDRESS                  0x34
#define RF_433_EN_FILT_MULTICAST_ADDRESS   S_DISABLE
#define RF_433_MULTICAST_ADDRESS           0xEE
#define RF_433_EN_FILT_BROADCAST_ADDRESS   S_DISABLE
#define RF_433_BROADCAST_ADDRESS           0xFF
#define RF_433_DESTINATION_ADDRESS         0x44

// Wake Up timer in ms for LDC mode
#define WAKEUP_TIMER                100.0

*/
