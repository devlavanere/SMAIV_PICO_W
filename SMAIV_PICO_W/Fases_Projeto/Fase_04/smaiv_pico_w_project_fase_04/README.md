# SMAIAS: Sistema de Monitoramento e Alerta Inteligente para Ambientes Sensíveis

Este é o repositório do projeto desenvolvido na Residência em TIC 37 – IFMA – EmbarcaTech para o desenvolvimento do SMAIAS, uma solução de baixo custo para monitoramento de ambientes sensíveis (quartos de bebês, idosos, etc.) baseada na placa de desenvolvimento BitDogLab com Raspberry Pi Pico W que usa o microcontrolador RP2040.

## Sobre o Projeto

O sistema utiliza o microfone embutido para detectar níveis sonoros elevados. Ao ultrapassar um limiar configurável, ele aciona alertas locais (LED RGB, Matriz de LEDs, Buzzer) e envia uma notificação remota via protocolo MQTT para um cuidador.

**Tecnologias Utilizadas:**
*   **Hardware:** Raspberry Pi Pico W, Placa BitDogLab
*   **Linguagem:** C/C++
*   **SDK:** Raspberry Pi Pico SDK
*   **Protocolo de Comunicação:** MQTT

## Fase 4: Conectividade Wi-Fi e MQTT

### Objetivo da Fase

O objetivo desta fase foi transformar o SMAIAS de um dispositivo local em um verdadeiro dispositivo de Internet das Coisas (IoT). A meta era implementar a conectividade Wi-Fi para conectar o sistema a uma rede e, em seguida, usar o protocolo MQTT para publicar mensagens de alerta em um broker, permitindo que cuidadores sejam notificados remotamente.

### Funcionalidades Implementadas

1.  **Conectividade Wi-Fi:**
    *   O módulo Wi-Fi do chip CYW43 na Raspberry Pi Pico W foi inicializado no modo `STA` (Station), permitindo que ele se conecte a um ponto de acesso existente.
    *   As credenciais da rede (SSID e senha) são gerenciadas através do arquivo de configuração `config.h`.

2.  **Cliente MQTT:**
    *   Foi integrado um cliente MQTT utilizando a biblioteca LwIP (Lightweight IP), que faz parte da Pico SDK.
    *   O sistema foi configurado para se conectar a um broker MQTT (para testes, foi utilizado o `broker.hivemq.com` e um broker local Mosquitto).

3.  **Publicação de Alertas Remotos:**
    *   Quando um evento de alerta é disparado (nível de som ultrapassa o limiar), o sistema agora formata uma mensagem JSON contendo o nível do som detectado.
    *   Esta mensagem é publicada no tópico MQTT pré-definido (`MQTT_TOPIC_ALERT`), tornando o alerta acessível a qualquer cliente MQTT (como um aplicativo de celular ou um dashboard) subscrito a este tópico.

4.  **Feedback Visual de Conectividade:**
    *   A interface do usuário no display OLED foi aprimorada para mostrar o status da conexão MQTT, exibindo "Conectando..." durante a tentativa e "OK" ou "Conectado" após o sucesso.
    *   O LED RGB também foi programado para refletir o status da rede, fornecendo um feedback rápido e intuitivo.

### Desafios Técnicos e Soluções

Durante esta fase, foram encontrados desafios significativos de conectividade, cuja resolução foi uma parte crucial do processo de desenvolvimento:

*   **Problema 1: Falha na Conexão Wi-Fi com Roteador Doméstico (`Erro -2: Rede não encontrada`)**
    *   **Diagnóstico:** Mesmo com as credenciais corretas, a Pico W não conseguia se conectar à rede principal. Para isolar o problema, o dispositivo foi testado no modo Access Point (AP), o que funcionou perfeitamente, provando que o hardware de rádio estava funcional.
    *   **Solução:** O problema foi contornado com sucesso utilizando um **hotspot de celular como ponto de acesso**. Isso indicou que a falha original se devia a uma incompatibilidade de configuração (provavelmente canal ou largura de canal) com o roteador doméstico.

*   **Problema 2: Falha na Conexão MQTT com Broker Público (`Erro 256: Conexão Recusada`)**
    *   **Diagnóstico:** Após conectar-se ao Wi-Fi, a conexão com o broker `broker.hivemq.com` falhava. As causas comuns (Client ID duplicado, polling da rede) foram investigadas.
    *   **Solução:** A solução definitiva foi refatorar a lógica de conexão para ser **assíncrona**. Em vez de tentar uma conexão MQTT imediatamente após a conexão Wi-Fi, o código agora utiliza o **cliente DNS da LwIP (`dns_gethostbyname`)** para primeiro resolver o nome do host do broker. A conexão MQTT é iniciada somente após o sucesso da resolução de DNS, através de uma função de callback. Esta abordagem eliminou problemas de temporização e garantiu uma conexão robusta.

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

- [ ] **Fase 5: Integração Final e Refatoração**
  - *Descrição:* Combinação de todos os módulos (incluindo buzzer e matriz de LEDs nos alertas) e refatoração do código para uma arquitetura modular e limpa.

## Próximos Passos (Fase 5)

Com a funcionalidade de ponta a ponta (detecção -> alerta local -> alerta remoto) validada, a próxima e última fase se concentrará em:
1.  Integrar os atuadores de alerta restantes (Buzzer e Matriz de LEDs).
2.  Refatorar o código-fonte monolítico para uma arquitetura modular limpa e profissional.

---
_**Status:** Concluída. O protótipo é agora um dispositivo IoT funcional, capaz de enviar notificações remotas em tempo real._

## Como Compilar

1.  Clone este repositório: `git clone https://github.com/devlavanere/SMAIAS-Pico-W.git`
2.  Para compilar uma fase específica, você pode usar o checkout da tag correspondente. Ex: `git checkout v2.0-fase2`.
3.  Configure o ambiente de desenvolvimento para a Raspberry Pi Pico SDK.
4.  Use o VS Code com as extensões do Pico para compilar e carregar o projeto.