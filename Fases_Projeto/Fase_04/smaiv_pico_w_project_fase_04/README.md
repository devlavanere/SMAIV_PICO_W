# Fase 4: Conectividade Wi-Fi e MQTT

![Status da Fase](https://img.shields.io/badge/status-concluída-brightgreen)

## Objetivo da Fase

O objetivo desta fase foi transformar o SMAIV de um dispositivo autônomo local em um verdadeiro dispositivo de **Internet das Coisas (IoT)**. A meta era implementar a conectividade Wi-Fi e, em seguida, usar o protocolo MQTT para publicar mensagens de alerta em um broker, permitindo que cuidadores sejam notificados remotamente em tempo real.

---

## Funcionalidades Implementadas

1.  **Conexão Wi-Fi:** O módulo CYW43 da Raspberry Pi Pico W foi configurado no modo `STA` (Station) para se conectar a um ponto de acesso, com as credenciais (SSID e senha) gerenciadas no arquivo `config.h`.

2.  **Cliente MQTT:** Foi integrado um cliente MQTT utilizando a biblioteca da pilha LwIP, parte do Pico C/C++ SDK, para se conectar a um broker público (`broker.hivemq.com`).

3.  **Publicação de Alertas Remotos:** Ao detectar um evento de som que ultrapassa o limiar, o sistema agora formata e publica uma mensagem JSON no tópico MQTT pré-definido. Esta mensagem contém dados relevantes, como o nível do som, tornando o alerta acessível a qualquer cliente subscrito, como um aplicativo de celular.

4.  **Feedback Visual de Conectividade:** A interface no display OLED foi aprimorada para exibir o status da conexão MQTT, e o LED RGB foi programado para fornecer um feedback rápido e intuitivo sobre o estado da rede (Wi-Fi e MQTT).

---

## Desafios Técnicos Superados

Esta fase apresentou desafios significativos de conectividade, cuja resolução foi crucial para o sucesso do projeto:

*   **Problema 1: Incompatibilidade com Roteador Wi-Fi**
    *   **Diagnóstico:** O dispositivo falhava ao tentar se conectar à rede Wi-Fi principal, embora o hardware de rádio estivesse funcional (validado com testes em modo Access Point).
    *   **Solução:** O problema foi contornado utilizando um **hotspot de celular**, o que indicou uma provável incompatibilidade de configuração com o roteador. Esta solução permitiu que o desenvolvimento prosseguisse de forma estável.

*   **Problema 2: Falha na Conexão com o Broker MQTT**
    *   **Diagnóstico:** Mesmo com a conexão Wi-Fi ativa, o cliente MQTT retornava um erro de "Conexão Recusada" ao tentar se conectar ao broker público.
    *   **Solução:** A causa raiz foi identificada como um problema de temporização na resolução de DNS. A solução definitiva foi refatorar a lógica de conexão para ser **assíncrona**: o sistema agora utiliza o cliente DNS da LwIP (`dns_gethostbyname`) para primeiro obter o endereço IP do broker e só então, dentro de uma função de *callback*, iniciar a conexão MQTT. Esta abordagem eliminou o erro e tornou a conexão robusta.

---

## Conclusão da Fase

Ao final desta fase, o protótipo SMAIV tornou-se um dispositivo IoT totalmente funcional. O ciclo completo, desde a detecção de som local até a notificação remota, foi validado com sucesso. A superação dos desafios de rede consolidou a base técnica para a integração final e otimização na última fase do projeto.

O desenvolvimento do projeto está sendo realizado em fases. O status atual é:

- [X] **Fase 1: Testes de Hardware (Concluída)**
  - *Descrição:* Todos os periféricos da placa BitDogLab (LEDs, botões, joystick, buzzer, OLED, microfone) foram testados e validados de forma interativa.

- [X] **Fase 2: Núcleo de Monitoramento (Concluída)**
  - *Descrição:* O sistema agora lê continuamente o microfone, calcula o nível de ruído ambiente (RMS) e o compara com um limiar pré-definido. Um alerta visual (LED RGB) é ativado quando o limiar é excedido.

- [x] **Fase 3: Interface do Usuário (UI)**
  - *Descrição:* Desenvolvimento da interface no display OLED para visualização de status e configuração do limiar de sensibilidade através do joystick.

- [x] **Fase 4: Conectividade MQTT**
  - *Descrição:* Implementação da conexão Wi-Fi e envio de alertas remotos via protocolo MQTT.

- [ ] **Fase 5: Integração Final e Refatoração para Dual-Core**
  - *Descrição:* Combinação de todos os módulos (incluindo buzzer e matriz de LEDs nos alertas) e refatoração do código para uma arquitetura modular e Dual-Core.

---
_**Status:** Concluída. O protótipo é agora um dispositivo IoT funcional, capaz de enviar notificações remotas em tempo real._
