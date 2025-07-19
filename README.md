# SMAIV - Sistema de Monitoramento Assistivo para Indivíduos Vulneráveis

![Status do Projeto](https://img.shields.io/badge/status-concluído-brightgreen)
![Plataforma](https://img.shields.io/badge/plataforma-Raspberry%20Pi%20Pico%20W-blue)
![Linguagem](https://img.shields.io/badge/linguagem-C/C++-orange)

Este repositório  é o repositório do projeto desenvolvido na Residência em TIC 37 – IFMA – EmbarcaTech e contém o código-fonte e a documentação do projeto SMAIV, um sistema de alerta com detecção de som, de valor acessível e código aberto, projetado para o monitoramento de ambientes com indivíduos vulneráveis (recém-nascidos, idosos, etc.). O projeto foi desenvolvido com foco na aplicação de conceitos de Sistemas Embarcados e Internet das Coisas (IoT).

O objetivo principal é oferecer uma alternativa acessível e flexível às soluções comerciais, além de servir como uma plataforma educacional para estudantes e entusiastas da área.

## Sobre o Projeto

O sistema utiliza o microfone embutido para detectar níveis sonoros elevados. Ao ultrapassar um limiar configurável, ele aciona alertas locais (LED RGB, Matriz de LEDs, Buzzer) e envia uma notificação remota via protocolo MQTT para uma pessoa inscrita no tópico.

---

## Principais Funcionalidades

- **Detecção de Som Inteligente:** Utiliza o cálculo de RMS (Root Mean Square) para detecção do nível de ruído ambiente.
- **Alertas Locais Multimodais:** Aciona um conjunto de alertas visuais (LED RGB, Matriz WS2812B) e sonoros (Buzzer) para notificação imediata.
- **Notificação Remota via IoT:** Envia alertas em tempo real para cuidadores através do protocolo MQTT, utilizando a conectividade Wi-Fi da Pico W.
- **Interface de Usuário Autônoma:** Possui um display OLED e controles (joystick/botões) que permitem a visualização do status e a configuração do limiar de sensibilidade sem a necessidade de um computador.
- **Arquitetura Dual-Core:** Otimizado para usar os dois núcleos do RP2040, com o Core 1 dedicado ao processamento de áudio e o Core 0 gerenciando a lógica principal, UI e rede, garantindo alta responsividade.
- **Código Aberto e Modular:** Projetado com uma arquitetura de software limpa e modular para facilitar a manutenção, a compreensão e futuras expansões.

---

## 🛠️ Hardware e Software

### Hardware Necessário
- **Raspberry Pi Pico W**
- **Placa de Desenvolvimento BitDogLab** (ou os seguintes componentes montados em protoboard):
  - Microfone Analógico (ex: MAX9814)
  - Display OLED 128x64 (SSD1306) via I2C
  - LED RGB
  - Matriz de LEDs WS2812B
  - Buzzer Passivo
  - Joystick e Botões

### Software e Ferramentas
- **Visual Studio Code** com a extensão para Raspberry Pi Pico.
- **Raspberry Pi Pico C/C++ SDK**.
- **CMake** e **GNU Arm Embedded Toolchain**.
- Um cliente MQTT (como o app **MQTT ClIENT**) para receber os alertas no celular.

---

---

## 🚀 Evolução do Desenvolvimento (Fases)

O projeto foi construído de forma incremental. Cada pasta no diretório `Fases/` contém uma versão funcional e autocontida do projeto em um determinado estágio, com seu próprio `README.md` detalhado.

- **[Fase 1: Validação de Hardware](./Fases_Prjeto/Fase_01/smaiv_pico_w_project_fase_01/README.md):** Teste interativo de todos os componentes de hardware.
- **[Fase 2: Núcleo de Monitoramento](./Fases_Prjeto/Fase_01/smaiv_pico_w_project_fase_02/README.md):** Implementação da detecção de som via RMS.
- **[Fase 3: Interface com o Usuário](./Fases_Prjeto/Fase_01/smaiv_pico_w_project_fase_03/README.md):** Adição do display OLED e controles para autonomia.
- **[Fase 4: Conectividade MQTT](./Fases_Prjeto/Fase_01/smaiv_pico_w_project_fase_04/README.md):** Implementação do alerta remoto via Wi-Fi.
- **[Fase 5: Sistema Integrado e Dual-Core](./Fases_Prjeto/Fase_01/smaiv_pico_w_project_fase_05/README.md):** Refatoração para arquitetura modular e otimização com uso dos dois núcleos.

---

## 🔧 Como Compilar e Executar (Versão Final)

1.  **Clone o repositório:**
    ```bash
    git clone https://github.com/devlavanere/SMAIV_PICO_W.git
    cd [NOME_DO_REPOSITORIO]
    ```

2.  **Navegue para a pasta da versão final:**
    ```bash
    cd Fases_Projeto/Fase_05/smaiv_pico_w_project_fase_05
    ```

3.  **Configure suas credenciais:**
    - Abra o arquivo `src/config.h`.
    - Insira o **SSID** e a **senha** da sua rede Wi-Fi.
    - Configure os detalhes do seu **Broker MQTT** e **Tópico**.

4.  **Compile o projeto no Windows** (usando o VS Code ou a linha de comando):
    ```bash
    mkdir build
    cd build
    cmake .. -G Ninja
    ninja
    ```

5.  **Carregue o firmware:**
    - Pressione e segure o botão `BOOTSEL` na sua Pico W e conecte-a ao computador.
    - Depois só apertar Run no Vs Code ou no plugin do SDK da PICO para carregar o programa no microlador RP2040.

---

## 🔮 Trabalhos Futuros e Evolução do Ecossistema SMAIV

O protótipo atual estabelece uma base sólida, mas seu verdadeiro potencial reside na evolução contínua. As direções futuras se concentram em tornar o sistema mais inteligente, seguro, acessível e expansível.

### 1. Inteligência Artificial Embarcada (TinyML)
A evolução mais impactante seria migrar da detecção de volume (RMS) para o **reconhecimento de padrões sonoros**.
- **Implementação:** Treinar e implantar um modelo de Machine Learning (TinyML) diretamente no microcontrolador.
- **Benefício:** Permitiria ao SMAIV diferenciar sons específicos, como **choro de bebê**, tosse, quebra de vidro ou até mesmo palavras de comando, reduzindo drasticamente os falsos positivos e tornando os alertas contextualmente relevantes.

### 2. Acessibilidade para Deficientes Auditivos
Expandir o ecossistema SMAIV para atender a cuidadores com deficiência auditiva, como mães e pais surdos.
- **Implementação:** Desenvolver um **dispositivo receptor complementar**. Este segundo dispositivo (que poderia ser um bracelete vibratório, um pequeno display de mesa ou um sinalizador luminoso) se comunicaria diretamente com a unidade principal do SMAIV (via Wi-Fi, Bluetooth ou outro rádio de baixo consumo).
- **Benefício:** Quando o SMAIV detectasse um choro, ele enviaria um sinal para o dispositivo receptor, que traduziria o alerta sonoro em um **sinal tátil (vibração) ou visual (luzes piscantes)**. Isso criaria uma solução de monitoramento verdadeiramente inclusiva, garantindo que o alerta seja percebido por todos.

### 3. Aplicativo Móvel Dedicado e Segurança
Aprimorar a experiência de monitoramento remoto e a segurança do sistema.
- **Aplicativo Móvel:** Desenvolver uma aplicação dedicada para Android e iOS que ofereça uma interface amigável para receber notificações push, visualizar um histórico de eventos, ver o nível de som em tempo real e configurar o dispositivo remotamente.
- **Segurança (TLS):** Implementar criptografia **TLS (Transport Layer Security)** na comunicação MQTT para garantir que todos os dados trocados entre o dispositivo e os cuidadores sejam privados e seguros.

---

## 👨‍💻 Autor

- **Michel L. Sampaio**

## 📜 Licença

Este projeto está sob a licença MIT. Veja o arquivo `LICENSE` para mais detalhes.
