#include "u8g2_esp8266_hal.h"

#include "ets_sys.h"
#include "osapi.h"
#include "driver/spi.h"
#include "driver/spi_interface.h"

void set_gpio(uint8_t gpio_num, uint8_t value) {
    if (value) {
        GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, 1 << gpio_num);
    } else {
        GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, 1 << gpio_num);
    }
}

uint8_t u8x8_gpio_and_delay_esp8266(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch(msg)
    {
        case U8X8_MSG_GPIO_AND_DELAY_INIT:  // called once during init phase of u8g2/u8x8
            ETS_GPIO_INTR_DISABLE();

            PIN_FUNC_SELECT(SSD1306_D0_MUX, SSD1306_D0_FUNC);
            PIN_FUNC_SELECT(SSD1306_D1_MUX, SSD1306_D1_FUNC);

            PIN_FUNC_SELECT(SSD1306_RESET_MUX, SSD1306_RESET_FUNC);
            PIN_FUNC_SELECT(SSD1306_CS_MUX, SSD1306_CS_FUNC);
            PIN_FUNC_SELECT(SSD1306_DC_MUX, SSD1306_DC_FUNC);

            uint32_t gpio_mask =
                (1 << SSD1306_D0_GPIO) | (1 << SSD1306_D1_GPIO) |
                (1 << SSD1306_RESET_GPIO) | (1 << SSD1306_DC_GPIO) |
                (1 << SSD1306_CS_GPIO);
            GPIO_REG_WRITE(GPIO_ENABLE_W1TS_ADDRESS, gpio_mask);
            GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, gpio_mask);

            ETS_GPIO_INTR_ENABLE();
            break;                            // can be used to setup pins
        case U8X8_MSG_DELAY_NANO:           // delay arg_int * 1 nano second
            // An approximation which seems to suffice
            os_delay_us(arg_int == 0 ? 0 : 1);
            break;
        case U8X8_MSG_DELAY_100NANO:        // delay arg_int * 100 nano seconds
            os_delay_us(arg_int / 10);
            break;
        case U8X8_MSG_DELAY_10MICRO:        // delay arg_int * 10 micro seconds
            os_delay_us(10 * arg_int);
            break;
        case U8X8_MSG_DELAY_MILLI:          // delay arg_int * 1 milli second
            os_delay_us(1000 * arg_int);
            break;
        case U8X8_MSG_GPIO_D0:              // D0 or SPI clock pin: Output level in arg_int
            set_gpio(SSD1306_D0_GPIO, arg_int);
            break;
        case U8X8_MSG_GPIO_D1:              // D1 or SPI data pin: Output level in arg_int
            set_gpio(SSD1306_D1_GPIO, arg_int);
            break;
        case U8X8_MSG_GPIO_CS:              // CS (chip select) pin: Output level in arg_int
            set_gpio(SSD1306_CS_GPIO, arg_int);
            break;
        case U8X8_MSG_GPIO_DC:              // DC (data/cmd, A0, register select) pin: Output level in arg_int
            set_gpio(SSD1306_DC_GPIO, arg_int);
            break;
        case U8X8_MSG_GPIO_RESET:           // Reset pin: Output level in arg_int
            set_gpio(SSD1306_RESET_GPIO, arg_int);
            break;
        default:
            u8x8_SetGPIOResult(u8x8, 1);          // default return value
            break;
    }
    return 1;
}

uint8_t u8x8_byte_esp8266_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) {
    uint8_t *data;
    SpiSubMode internal_spi_mode;
    static SpiAttr attr = {
        .mode = SpiMode_Master,
        .bitOrder = SpiBitOrder_MSBFirst,
        .subMode = SpiSubMode_0,
        .speed = 20  // 4 MHz
    };
    switch(msg) {
        case U8X8_MSG_BYTE_SEND:
            data = (uint8_t *)arg_ptr;
            while( arg_int > 0 ) {
                while (READ_PERI_REG(SPI_CMD(HSPI)) & SPI_USR);
                CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_USR_MOSI | SPI_USR_MISO);

                WRITE_PERI_REG(SPI_USER2(HSPI),
                    ((7 & SPI_USR_COMMAND_BITLEN) << SPI_USR_COMMAND_BITLEN_S) |
                    ((uint32)*data));

                SET_PERI_REG_MASK(SPI_CMD(HSPI), SPI_USR);
                while (READ_PERI_REG(SPI_CMD(HSPI)) & SPI_USR);

                data++;
                arg_int--;
            }
            break;
        case U8X8_MSG_BYTE_INIT:
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);

            WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2);
            PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2);

            break;
        case U8X8_MSG_BYTE_SET_DC:
            u8x8_gpio_SetDC(u8x8, arg_int);
            break;
        case U8X8_MSG_BYTE_START_TRANSFER:
            internal_spi_mode = SpiSubMode_0;
            switch(u8x8->display_info->spi_mode) {
                case 0: internal_spi_mode = SpiSubMode_0; break;
                case 1: internal_spi_mode = SpiSubMode_1; break;
                case 2: internal_spi_mode = SpiSubMode_2; break;
                case 3: internal_spi_mode = SpiSubMode_3; break;
            }
            attr.subMode = internal_spi_mode;
            attr.speed = 80000000UL / u8x8->display_info->sck_clock_hz;
            SPIInit(SpiNum_HSPI, &attr);
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
            break;
        case U8X8_MSG_BYTE_END_TRANSFER:
            u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->pre_chip_disable_wait_ns, NULL);
            u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
            break;
        default:
            return 0;
    }
    return 1;
}

