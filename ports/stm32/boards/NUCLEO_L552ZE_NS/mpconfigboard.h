#define MICROPY_HW_BOARD_NAME       "NUCLEO-L552ZE"
#define MICROPY_HW_MCU_NAME         "STM32L552xx"

#define MICROPY_HW_ENABLE_INTERNAL_FLASH_STORAGE (1)
#define MICROPY_HW_HAS_SWITCH       (0)
#define MICROPY_HW_HAS_FLASH        (1)
#define MICROPY_HW_ENABLE_RNG       (0)
#define MICROPY_HW_ENABLE_RTC       (0)
#define MICROPY_HW_ENABLE_USB       (0)
#define MICROPY_HW_ENABLE_ADC       (0)
#define MICROPY_HW_ENABLE_CAN       (0)
#define MICROPY_HW_ENABLE_FDCAN     (0)

#define MICROPY_HW_CLK_PLLM (1)
#define MICROPY_HW_CLK_PLLN (55)
#define MICROPY_HW_CLK_PLLP (7)
#define MICROPY_HW_CLK_PLLQ (2)
#define MICROPY_HW_CLK_PLLR (2)

#define MICROPY_HW_FLASH_LATENCY    FLASH_LATENCY_5

// UART config
// #define MICROPY_HW_UART1_TX     (pin_G7)
// #define MICROPY_HW_UART1_RX     (pin_G8)

#define MICROPY_HW_UART2_TX     (pin_D5)
#define MICROPY_HW_UART2_RX     (pin_D6)

#define MICROPY_HW_UART3_TX     (pin_D8)
#define MICROPY_HW_UART3_RX     (pin_D9)

#define MICROPY_HW_UART_REPL        PYB_UART_3
#define MICROPY_HW_UART_REPL_BAUD   115200

// I2C busses
#define MICROPY_HW_I2C1_SCL (pin_B8)
#define MICROPY_HW_I2C1_SDA (pin_B9)

#define MICROPY_HW_I2C2_SCL (pin_F1)
#define MICROPY_HW_I2C2_SDA (pin_F0)

// SPI busses
#define MICROPY_HW_SPI1_NSS     (pin_D14)
#define MICROPY_HW_SPI1_SCK     (pin_A5)
#define MICROPY_HW_SPI1_MISO    (pin_A6)
#define MICROPY_HW_SPI1_MOSI    (pin_A7)

#define MICROPY_HW_SPI2_NSS     (pin_A4)
#define MICROPY_HW_SPI2_SCK     (pin_B3)
#define MICROPY_HW_SPI2_MISO    (pin_B4)
#define MICROPY_HW_SPI2_MOSI    (pin_B5)

// CAN busses
// #define MICROPY_HW_CAN1_TX (pin_D1)
// #define MICROPY_HW_CAN1_RX (pin_D0)

// USRSW is pulled low. Pressing the button makes the input go high.
// #define MICROPY_HW_USRSW_PIN        (pin_C13)
// #define MICROPY_HW_USRSW_PULL       (GPIO_NOPULL)
// #define MICROPY_HW_USRSW_EXTI_MODE  (GPIO_MODE_IT_RISING)
// #define MICROPY_HW_USRSW_PRESSED    (1)

// LEDs
#define MICROPY_HW_LED1             (pin_C7) // green
#define MICROPY_HW_LED2             (pin_B7) // blue
#define MICROPY_HW_LED3             (pin_A9) // red
#define MICROPY_HW_LED_ON(pin)      (mp_hal_pin_high(pin))
#define MICROPY_HW_LED_OFF(pin)     (mp_hal_pin_low(pin))

// USB config
// #define MICROPY_HW_USB_FS              (1)
// #define MICROPY_HW_USB_VBUS_DETECT_PIN (pin_C2)
// #define MICROPY_HW_USB_OTG_ID_PIN      (pin_A10)
