#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "config.h"

// Includes para periféricos específicos
#include "ssd1306/ssd1306.h"
#include "ws2812.pio.h"

// Função para pausar a execução e esperar o enter do usuário
void wait_for_button_A() {
    printf(">> Pressione o BOTAO A na placa para continuar...\n");
    while (gpio_get(BUTTON_A_PIN) == 1) {
        tight_loop_contents(); // Um loop seguro para esperar
    }
    // Debounce: esperamos um pouco após o pressionamento para evitar leituras múltiplas.
    sleep_ms(200);
}

// --- Funções de Controle de Hardware (WS2812) ---
static inline void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin, float freq, bool rgbw) {
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_out_shift(&c, false, true, rgbw ? 32 : 24);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    int cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;
    float div = clock_get_hz(clk_sys) / (freq * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}


int main() {
    stdio_init_all();
    
    // Espera até que o host (PC) tenha estabelecido a conexão serial USB.
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }

    printf("\n\n--- INICIANDO TESTE DE HARDWARE INTERATIVO (FASE 1) ---\n");
    printf("Conexao serial estabelecida.\n");
    printf("Cada teste sera executado apos voce pressionar o BOTAO A na placa.\n");
    wait_for_button_A();

    // --- TESTE 1: LED RGB ---
    printf("\n1. Testando LED RGB...\n");
    printf("   O LED RGB da placa vai piscar em VERMELHO, VERDE e AZUL.\n");
    gpio_init(RGB_R_PIN); gpio_set_dir(RGB_R_PIN, GPIO_OUT);
    gpio_init(RGB_G_PIN); gpio_set_dir(RGB_G_PIN, GPIO_OUT);
    gpio_init(RGB_B_PIN); gpio_set_dir(RGB_B_PIN, GPIO_OUT);
    gpio_put(RGB_R_PIN, 1); sleep_ms(700); gpio_put(RGB_R_PIN, 0);
    gpio_put(RGB_G_PIN, 1); sleep_ms(700); gpio_put(RGB_G_PIN, 0);
    gpio_put(RGB_B_PIN, 1); sleep_ms(700); gpio_put(RGB_B_PIN, 0);
    printf("   Teste LED RGB concluido.\n");
    wait_for_button_A();

    // --- TESTE 2: Botões e Joystick ---
    printf("\n2. Testando Botões e Botão Joystick...\n");
    printf("   Pressione os botoes B e o do joystick. O valor deve ir de 1 para 0.\n");
    printf("   O teste durara 10 segundos.\n");
    gpio_init(BUTTON_A_PIN); gpio_set_dir(BUTTON_A_PIN, GPIO_IN); gpio_pull_up(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN); gpio_set_dir(BUTTON_B_PIN, GPIO_IN); gpio_pull_up(BUTTON_B_PIN);
    gpio_init(JOYSTICK_SW_PIN); gpio_set_dir(JOYSTICK_SW_PIN, GPIO_IN); gpio_pull_up(JOYSTICK_SW_PIN);
    for (int i = 0; i < 100; i++) {
        printf("\r   Botao A: %d | Botao B: %d | Joy SW: %d", 
            gpio_get(BUTTON_A_PIN) ? 1 : 0, 
            gpio_get(BUTTON_B_PIN) ? 1 : 0, 
            gpio_get(JOYSTICK_SW_PIN) ? 1 : 0
        );
        fflush(stdout);
        sleep_ms(100);
    }
    printf("\n   Teste Botoes concluido.\n");
    wait_for_button_A();

    // --- TESTE 3: Microfone e Joystick (Pinos Analógicos) ---
    printf("\n3. Testando ADC (Microfone e Joystick X/Y)...\n");
    printf("   Faca barulho perto do microfone e mova o joystick. Os valores devem mudar.\n");
    printf("   O teste durara 10 segundos.\n");
    adc_init();
    adc_gpio_init(MIC_ADC_PIN);
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);
    for (int i = 0; i < 100; i++) {
        adc_select_input(MIC_ADC_INPUT);
        uint16_t mic_val = adc_read();
        adc_select_input(1);
        uint16_t joy_x_val = adc_read();
        adc_select_input(0);
        uint16_t joy_y_val = adc_read();
        printf("\r   MIC: %4d | Joy X: %4d | Joy Y: %4d", mic_val, joy_x_val, joy_y_val);
        fflush(stdout);
        sleep_ms(100);
    }
    printf("\n   Teste ADC concluido.\n");
    wait_for_button_A();

    // --- TESTE 4: Buzzer (Usando o Método PWM Config Struct) ---
    printf("\n4. Testando Buzzer (Metodo PWM Config Struct)...\n");
    printf("   Voce deve ouvir um som audivel por 2 segundos.\n");

    // Configura o pino para a função PWM
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    
    // Encontra qual "slice" de PWM controla este pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // Obtém a configuração padrão de PWM
    pwm_config config = pwm_get_default_config();
    
    // Configura o divisor de clock e o valor de wrap para definir a frequência
    // Clock de 125MHz / 100 (div) = 1,250,000 Hz
    // 1,250,000 Hz / 500 (wrap) = 2500 Hz = 2.5 kHz
    pwm_config_set_clkdiv(&config, 100.0f);
    pwm_config_set_wrap(&config, 500);
    
    // Inicializa o slice de PWM com a configuração e já o habilita
    pwm_init(slice_num, &config, true);

    // Inicia o som definindo o duty cycle para 50% (metade do wrap)
    pwm_set_gpio_level(BUZZER_PIN, 250);
    sleep_ms(2000); // Toca por 2 segundos
    
    // Para o som definindo o duty cycle para 0%
    pwm_set_gpio_level(BUZZER_PIN, 0);

    printf("   Teste Buzzer concluido.\n");
    wait_for_button_A();

    // --- TESTE 5: Matriz de LEDs WS2812 ---
    printf("\n5. Testando Matriz WS2812...\n");
    printf("   O primeiro LED da matriz vai piscar em VERDE, VERMELHO e AZUL.\n");
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, false);
    put_pixel(0x00FF00); // Verde
    sleep_ms(700);
    put_pixel(0xFF0000); // Vermelho
    sleep_ms(700);
    put_pixel(0x0000FF); // Azul
    sleep_ms(700);
    put_pixel(0x000000); // Desliga
    printf("   Teste Matriz WS2812 concluido.\n");
    wait_for_button_A();

    // --- TESTE 6: Display OLED ---
    printf("\n6. Testando Display OLED...\n");
    printf("   A mensagem 'FASE 1: OK' deve aparecer no display.\n");
    i2c_init(OLED_I2C_PORT, 400 * 1000);
    gpio_set_function(OLED_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(OLED_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(OLED_SDA_PIN);
    gpio_pull_up(OLED_SCL_PIN);
    
    ssd1306_t disp;
    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 64, 0x3C, OLED_I2C_PORT);
    ssd1306_clear(&disp);
    ssd1306_draw_string(&disp, 0, 0, 2, "SMAIV");
    ssd1306_draw_string(&disp, 0, 24, 1, "Teste de Hardware");
    ssd1306_draw_string(&disp, 0, 40, 1, "FASE 1: OK");
    ssd1306_show(&disp);
    printf("   Teste Display OLED concluido.\n");
    
    printf("\n\n--- TODOS OS TESTES FORAM CONCLUIDOS COM SUCESSO! ---\n");

    while(1) {
        tight_loop_contents();
    }
}