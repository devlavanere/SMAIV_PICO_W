# SMAIAS: Sistema de Monitoramento e Alerta Inteligente para Ambientes Sensíveis

Este é o repositório do projeto desenvolvido na Residência em TIC 37 – IFMA – EmbarcaTech para o desenvolvimento do SMAIAS, uma solução de baixo custo para monitoramento de ambientes sensíveis (quartos de bebês, idosos, etc.) baseada na placa de desenvolvimento BitDogLab com Raspberry Pi Pico W que usa o microcontrolador RP2040.

## Sobre o Projeto

O sistema utiliza o microfone embutido para detectar níveis sonoros elevados. Ao ultrapassar um limiar configurável, ele aciona alertas locais (LED RGB, Matriz de LEDs, Buzzer) e envia uma notificação remota via protocolo MQTT para um cuidador.

**Tecnologias Utilizadas:**
*   **Hardware:** Raspberry Pi Pico W, Placa BitDogLab
*   **Linguagem:** C/C++
*   **SDK:** Raspberry Pi Pico SDK
*   **Protocolo de Comunicação:** MQTT

O desenvolvimento do projeto está sendo realizado em fases. O status atual é:

- [X] **Fase 1: Testes de Hardware (Concluída)**
  - *Descrição:* Todos os periféricos da placa BitDogLab (LEDs, botões, joystick, buzzer, OLED, microfone) foram testados e validados de forma interativa.
  - *Tag no Git:* `v1.0-fase1`

- [X] **Fase 2: Núcleo de Monitoramento (Concluída)**
  - *Descrição:* O sistema agora lê continuamente o microfone, calcula o nível de ruído ambiente (RMS) e o compara com um limiar pré-definido. Um alerta visual (LED RGB) é ativado quando o limiar é excedido.
  - *Tag no Git:* `v2.0-fase2`

- [ ] **Fase 3: Interface do Usuário (UI)**
  - *Descrição:* Desenvolvimento da interface no display OLED para visualização de status e configuração do limiar de sensibilidade através do joystick.

- [ ] **Fase 4: Conectividade MQTT**
  - *Descrição:* Implementação da conexão Wi-Fi e envio de alertas remotos via protocolo MQTT.

- [ ] **Fase 5: Integração Final e Refatoração**
  - *Descrição:* Combinação de todos os módulos (incluindo buzzer e matriz de LEDs nos alertas) e refatoração do código para uma arquitetura modular e limpa.

## Como Compilar

1.  Clone este repositório: `git clone https://github.com/devlavanere/SMAIAS-Pico-W.git`
2.  Para compilar uma fase específica, você pode usar o checkout da tag correspondente. Ex: `git checkout v2.0-fase2`.
3.  Configure o ambiente de desenvolvimento para a Raspberry Pi Pico SDK.
4.  Use o VS Code com as extensões do Pico para compilar e carregar o projeto.