# SMAIAS: Sistema de Monitoramento e Alerta Inteligente para Ambientes Sensíveis

Este é o repositório do projeto desenvolvido na Residência em TIC 37 – IFMA – EmbarcaTech para o desenvolvimento do SMAIAS, uma solução de baixo custo para monitoramento de ambientes sensíveis (quartos de bebês, idosos, etc.) baseada na placa de desenvolvimento BitDogLab com Raspberry Pi Pico W que usa o microcontrolador RP2040.

## Sobre o Projeto

O sistema utiliza o microfone embutido para detectar níveis sonoros elevados. Ao ultrapassar um limiar configurável, ele aciona alertas locais (LED RGB, Matriz de LEDs, Buzzer) e envia uma notificação remota via protocolo MQTT para um cuidador.

**Tecnologias Utilizadas:**
*   **Hardware:** Raspberry Pi Pico W, Placa BitDogLab
*   **Linguagem:** C/C++
*   **SDK:** Raspberry Pi Pico SDK
*   **Protocolo de Comunicação:** MQTT

## Fase 3: Interface do Usuário (UI)

### Objetivo da Fase

O objetivo desta fase foi transformar o SMAIAS em um dispositivo autônomo e interativo, adicionando uma interface de usuário local. Isso foi alcançado integrando um display OLED e lendo as entradas do joystick e botões da placa BitDogLab.

O núcleo de monitoramento de áudio da Fase 2 foi mantido e agora suas informações são exibidas visualmente para o usuário.

### Funcionalidades Implementadas

1.  **Integração do Display OLED:**
    *   O display OLED (baseado no controlador SSD1306) foi inicializado e configurado para comunicação via I2C.
    *   Foi utilizada a biblioteca `pico-ssd1306` para abstrair as operações de desenho.

2.  **Visualização de Status em Tempo Real:**
    *   A tela principal exibe continuamente o nome do projeto ("SMAIAS").
    *   Mostra o **nível de ruído (RMS)** atual, lido do microfone.
    *   Mostra o **limiar de sensibilidade** configurado.
    *   Exibe o status do sistema: "Monitorando..." em estado normal e um grande **"ALERTA!"** quando o nível de ruído ultrapassa o limiar.

3.  **Sistema de Menus e Navegação:**
    *   Foi implementada uma máquina de estados simples com duas telas: `SCREEN_MAIN` (principal) e `SCREEN_SETTINGS` (ajustes).
    *   O **botão do joystick (SW)** é usado para navegar da tela principal para a tela de ajustes.

4.  **Configuração de Sensibilidade no Dispositivo:**
    *   Na tela de ajustes, o **eixo Y do joystick** é utilizado para aumentar ou diminuir o valor da variável `sound_threshold`.
    *   O **Botão A** é usado para "salvar" (confirmar) o novo valor e retornar à tela principal.
    *   Isso permite que o usuário calibre a sensibilidade do SMAIAS diretamente no hardware, sem a necessidade de um computador ou de recompilar o código.

### Detalhes Técnicos

*   **Pinos Utilizados (Novos nesta Fase):**
    *   `GPIO14` (SDA) e `GPIO15` (SCL) para o barramento I2C do display OLED.
    *   `GPIO26` (ADC0) para o eixo Y do joystick.
    *   `GPIO22` para o botão de switch do joystick.
    *   `GPIO5` para o Botão A.

*   **Lógica de Controle:**
    *   O `main loop` foi reestruturado para seguir um fluxo claro:
        1.  `handle_input()`: Processar as entradas do usuário primeiro.
        2.  `get_sound_level()`: Ler os sensores e atualizar a lógica de alerta.
        3.  `draw_screen()`: Atualizar todos os atuadores (LEDs e display) com o novo estado do sistema.

O desenvolvimento do projeto está sendo realizado em fases. O status atual é:

- [X] **Fase 1: Testes de Hardware (Concluída)**
  - *Descrição:* Todos os periféricos da placa BitDogLab (LEDs, botões, joystick, buzzer, OLED, microfone) foram testados e validados de forma interativa.
  - *Tag no Git:* `v1.0-fase1`

- [X] **Fase 2: Núcleo de Monitoramento (Concluída)**
  - *Descrição:* O sistema agora lê continuamente o microfone, calcula o nível de ruído ambiente (RMS) e o compara com um limiar pré-definido. Um alerta visual (LED RGB) é ativado quando o limiar é excedido.
  - *Tag no Git:* `v2.0-fase2`

- [x] **Fase 3: Interface do Usuário (UI)**
  - *Descrição:* Desenvolvimento da interface no display OLED para visualização de status e configuração do limiar de sensibilidade através do joystick.
  - *Tag no Git:* `v2.0-fase3`

- [ ] **Fase 4: Conectividade MQTT**
  - *Descrição:* Implementação da conexão Wi-Fi e envio de alertas remotos via protocolo MQTT.

- [ ] **Fase 5: Integração Final e Refatoração**
  - *Descrição:* Combinação de todos os módulos (incluindo buzzer e matriz de LEDs nos alertas) e refatoração do código para uma arquitetura modular e limpa.

## Como Compilar

1.  Clone este repositório: `git clone https://github.com/devlavanere/SMAIAS-Pico-W.git`
2.  Para compilar uma fase específica, você pode usar o checkout da tag correspondente. Ex: `git checkout v2.0-fase2`.
3.  Configure o ambiente de desenvolvimento para a Raspberry Pi Pico SDK.
4.  Use o VS Code com as extensões do Pico para compilar e carregar o projeto.