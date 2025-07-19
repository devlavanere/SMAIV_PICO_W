# SMAIAS: Sistema de Monitoramento e Alerta Inteligente para Ambientes Sensíveis

Este é o repositório do projeto desenvolvido na Residência em TIC 37 – IFMA – EmbarcaTech para o desenvolvimento do SMAIAS, uma solução de baixo custo para monitoramento de ambientes sensíveis (quartos de bebês, idosos, etc.) baseada na placa de desenvolvimento BitDogLab com Raspberry Pi Pico W que usa o microcontrolador RP2040.

## Sobre o Projeto

O sistema utiliza o microfone embutido para detectar níveis sonoros elevados. Ao ultrapassar um limiar configurável, ele aciona alertas locais (LED RGB, Matriz de LEDs, Buzzer) e envia uma notificação remota via protocolo MQTT para um cuidador.

**Tecnologias Utilizadas:**
*   **Hardware:** Raspberry Pi Pico W, Placa BitDogLab
*   **Linguagem:** C/C++
*   **SDK:** Raspberry Pi Pico SDK
*   **Protocolo de Comunicação:** MQTT

## Status Atual do Desenvolvimento

*   **[X] Fase 1: Testes de Hardware:** Todos os periféricos da placa BitDogLab (LEDs, botões, joystick, buzzer, OLED, microfone) foram testados e validados de forma interativa.
*   **[ ] Fase 2: Núcleo de Monitoramento:** Implementação da lógica de leitura de áudio e detecção de limiar.
*   **[ ] Fase 3: Interface do Usuário (UI):** Desenvolvimento da interface no display OLED com menus de configuração.
*   **[ ] Fase 4: Conectividade MQTT:** Implementação da conexão Wi-Fi e envio de alertas remotos.
*   **[ ] Fase 5: Integração Final e Refatoração:** Combinação de todos os módulos e organização do código-fonte.

## Como Compilar

1. Clone este repositório.
2. Configure o ambiente de desenvolvimento para a Raspberry Pi Pico SDK.
3. Use o VS Code com as extensões do Pico para compilar e carregar o projeto.