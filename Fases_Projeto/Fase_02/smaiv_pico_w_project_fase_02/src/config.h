#ifndef CONFIG_H
#define CONFIG_H

// --- Definição dos GPIOs ---

// Microfone (ADC)
#define MIC_ADC_PIN 28
#define MIC_ADC_INPUT 2 // ADC2 corresponde ao GPIO28

// Display OLED (I2C)
#define OLED_I2C_PORT i2c1
#define OLED_SDA_PIN 14
#define OLED_SCL_PIN 15

// Joystick
#define JOYSTICK_X_PIN 27
#define JOYSTICK_Y_PIN 26
#define JOYSTICK_SW_PIN 22

// Botões
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

// LED RGB
#define RGB_R_PIN 13
#define RGB_G_PIN 11
#define RGB_B_PIN 12

// Buzzer
#define BUZZER_PIN 21

// Matriz de LEDs WS2812B (NeoPixel)
#define WS2812_PIN 7
#define WS2812_NUM_LEDS 16 // Matriz 4x4

#endif