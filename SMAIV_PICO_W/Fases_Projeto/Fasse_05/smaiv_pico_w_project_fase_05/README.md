# SMAIAS: Sistema de Monitoramento e Alerta Inteligente para Ambientes Sensíveis

Este é o repositório do projeto desenvolvido na Residência em TIC 37 – IFMA – EmbarcaTech para o desenvolvimento do SMAIAS, uma solução de baixo custo para monitoramento de ambientes sensíveis (quartos de bebês, idosos, etc.) baseada na placa de desenvolvimento BitDogLab com Raspberry Pi Pico W que usa o microcontrolador RP2040.

## Sobre o Projeto

O sistema utiliza o microfone embutido para detectar níveis sonoros elevados. Ao ultrapassar um limiar configurável, ele aciona alertas locais (LED RGB, Matriz de LEDs, Buzzer) e envia uma notificação remota via protocolo MQTT para um cuidador.

**Tecnologias Utilizadas:**
*   **Hardware:** Raspberry Pi Pico W, Placa BitDogLab
*   **Linguagem:** C/C++
*   **SDK:** Raspberry Pi Pico SDK
*   **Protocolo de Comunicação:** MQTT

## Fase 5: Integração Final e Refatoração

### Objetivo da Fase

Esta é a fase de conclusão do protótipo do SMAIAS. O objetivo foi duplo:
1.  **Integrar todos os atuadores de alerta** planejados, criando um sistema de notificação local completo e multimodal.
2.  **Refatorar o código-fonte monolítico**, que foi desenvolvido nas fases anteriores, para uma **arquitetura de software modular**, seguindo as melhores práticas de engenharia para garantir manutenibilidade, legibilidade e escalabilidade.

### Funcionalidades Concluídas

Esta versão representa o protótipo completo, com todas as funcionalidades integradas:

1.  **Alertas Locais Multimodais:**
    *   Além do LED RGB, o sistema agora aciona um **buzzer sonoro** e uma **matriz de LEDs WS2812B** quando um alerta é disparado.
    *   A matriz de LEDs foi configurada para acender com uma cor vermelha de brilho moderado, uma solução de software implementada para contornar as limitações de corrente da alimentação via USB.

2.  **Lógica de Alarme com Travamento (Latching):**
    *   O sistema foi aprimorado para se comportar como um alarme real. Uma vez que o limiar de som é ultrapassado, o dispositivo entra em um estado de "Alarme Ativo" e **permanece nesse estado** até ser resetado.
    *   O reset do alarme é feito manualmente pelo usuário, pressionando o **Botão A**, o que simula a ação de um cuidador reconhecendo e atendendo ao alerta.

### Arquitetura de Software Modular

A mudança mais significativa nesta fase foi a **refatoração do código**. O arquivo `main.c` monolítico foi dividido em módulos lógicos, cada um com sua própria responsabilidade bem definida. O `main.c` agora atua como um **orquestrador**, coordenando as chamadas para os diferentes módulos.

A nova estrutura do código-fonte é:

*   `src/main.c`: O orquestrador principal e ponto de entrada da aplicação.
*   `src/common.h`: Define a estrutura de estado global `system_state_t`, compartilhada entre os módulos.
*   **Módulos:**
    *   `modules/audio_processing/`: Encapsula a lógica de leitura do ADC e o cálculo do RMS.
    *   `modules/ui_manager/`: Gerencia todo o ciclo de vida da interface do usuário (display OLED, botões, joystick).
    *   `modules/local_alerts/`: Controla todos os atuadores de alerta físico (LED RGB, Buzzer, Matriz WS2812B).
    *   `modules/mqtt_comm/`: Lida com a conectividade de rede e a comunicação MQTT.

Esta arquitetura resulta em um código mais limpo, mais fácil de depurar e muito mais simples de estender com novas funcionalidades no futuro.

### Detalhes Técnicos Finais

*   **Gerenciamento de Estado:** A comunicação entre os módulos é feita através de um ponteiro para uma única estrutura de estado (`system_state_t`), garantindo que todos os componentes operem com base em dados consistentes.
*   **Fluxo de Execução:** O loop principal agora segue um padrão claro: ler entradas do usuário, ler sensores, atualizar a lógica de negócio (estado de alarme), e por fim, atualizar todos os atuadores e a tela.
*   **Controle de Consumo:** Foi implementado o controle de brilho via software na matriz de LEDs para garantir a operação estável sob a limitação de corrente da porta USB.

### Conclusão do Protótipo

Ao final desta fase, o SMAIAS é um protótipo completo e funcional que atende a todos os objetivos propostos. Ele demonstra a viabilidade de construir um sistema de monitoramento inteligente e de baixo custo, com uma base de código robusta e profissional, pronta para futuras evoluções.


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

- [x] **Fase 4: Conectividade MQTT**
  - *Descrição:* Implementação da conexão Wi-Fi e envio de alertas remotos via protocolo MQTT.

- [x] **Fase 5: Integração Final e Refatoração**
  - *Descrição:* Combinação de todos os módulos (incluindo buzzer e matriz de LEDs nos alertas) e refatoração do código para uma arquitetura modular e limpa.

---
_**Status:** Concluída. Protótipo finalizado e refatorado._

## Como Compilar

1.  Clone este repositório: `git clone https://github.com/devlavanere/SMAIAS-Pico-W.git`
2.  Para compilar uma fase específica, você pode usar o checkout da tag correspondente. Ex: `git checkout v2.0-fase2`.
3.  Configure o ambiente de desenvolvimento para a Raspberry Pi Pico SDK.
4.  Use o VS Code com as extensões do Pico para compilar e carregar o projeto.