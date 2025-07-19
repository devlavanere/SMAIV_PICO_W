# Fase 1: Validação de Hardware

## Objetivo da Fase
O objetivo desta fase inicial foi validar o funcionamento de todos os periféricos de hardware da placa BitDogLab, garantindo que a comunicação entre cada componente e o microcontrolador RP2040 estivesse correta.

## Funcionalidades Implementadas
- Um firmware de teste interativo que verifica, um a um, os seguintes componentes:
  - LED RGB
  - Botões (A, B e do Joystick)
  - ADC (lendo valores do microfone e dos eixos do joystick)
  - Buzzer (gerando um tom via PWM)
  - Matriz de LEDs WS2812B (controlada via PIO)
  - Display OLED (exibindo uma mensagem de sucesso)

## Como Executar
1. Compile o projeto contido nesta pasta.
2. Carregue o arquivo `.uf2` na sua Raspberry Pi Pico W.
3. Abra um monitor serial (ex: no VS Code ou PuTTY).
4. Pressione o **Botão A** na placa para avançar por cada etapa do teste.

## Saída Esperada no Console
A saída no monitor serial guiará o usuário através dos testes, confirmando a conclusão de cada um:

```
--- INICIANDO TESTE DE HARDWARE INTERATIVO (FASE 1) ---
Conexao serial estabelecida.
Cada teste sera executado apos voce pressionar o BOTAO A na placa.
>> Pressione o BOTAO A na placa para continuar...

1. Testando LED RGB...
   O LED RGB da placa vai piscar em VERMELHO, VERDE e AZUL.
   Teste LED RGB concluido.
>> Pressione o BOTAO A na placa para continuar...

... (restante dos testes) ...

6. Testando Display OLED...
   A mensagem 'FASE 1: OK' deve aparecer no display.
   Teste Display OLED concluido.

--- TODOS OS TESTES FORAM CONCLUIDOS COM SUCESSO! ---
```

## Status Atual do Desenvolvimento

*   **[X] Fase 1: Testes de Hardware:** Todos os periféricos da placa BitDogLab (LEDs, botões, joystick, buzzer, OLED, microfone) foram testados e validados de forma interativa.
*   **[ ] Fase 2: Núcleo de Monitoramento:** Implementação da lógica de leitura de áudio e detecção de limiar.
*   **[ ] Fase 3: Interface do Usuário (UI):** Desenvolvimento da interface no display OLED com menus de configuração.
*   **[ ] Fase 4: Conectividade MQTT:** Implementação da conexão Wi-Fi e envio de alertas remotos.
*   **[ ] Fase 5: Integração Final e Refatoração:** Combinação de todos os módulos e organização do código-fonte.
