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
- Um cliente MQTT (como o app **MQTT Dash**) para receber os alertas no celular.

---

---

## 🚀 Evolução do Desenvolvimento (Fases)

O projeto foi construído de forma incremental. Cada pasta no diretório `Fases/` contém uma versão funcional e autocontida do projeto em um determinado estágio, com seu próprio `README.md` detalhado.

- **[Fase 1: Validação de Hardware](./Fases_Prjeto/Fase_1_Validacao_Hardware/README.md):** Teste interativo de todos os componentes de hardware.
- **[Fase 2: Núcleo de Monitoramento](./Fases_Prjeto/Fase_2_Nucleo_Monitoramento/README.md):** Implementação da detecção de som via RMS.
- **[Fase 3: Interface com o Usuário](./Fases_Prjeto/Fase_3_Interface_Usuario/README.md):** Adição do display OLED e controles para autonomia.
- **[Fase 4: Conectividade MQTT](./Fases_Prjeto/Fase_4_Conectividade_MQTT/README.md):** Implementação do alerta remoto via Wi-Fi.
- **[Fase 5: Sistema Integrado e Dual-Core](./Fases_Prjeto/Fase_5_Sistema_Integrado_DualCore/README.md):** Refatoração para arquitetura modular e otimização com uso dos dois núcleos.

---

## 🔧 Como Compilar e Executar (Versão Final)

1.  **Clone o repositório:**
    ```bash
    git clone [URL_DO_SEU_REPOSITORIO]
    cd [NOME_DO_REPOSITORIO]
    ```

2.  **Navegue para a pasta da versão final:**
    ```bash
    cd Fases/Fase_5_Sistema_Integrado_DualCore
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
    - Arraste o arquivo `smaiv_fase_5.uf2` (gerado na pasta `build`) para o drive da Pico.

---

## 🔮 Trabalhos Futuros

- **Inteligência Artificial (TinyML):** Implementar um modelo de classificação de áudio para diferenciar sons específicos (ex: choro de bebê, quebra de vidro).
- **Segurança:** Adicionar criptografia TLS à comunicação MQTT.
- **Aplicativo Móvel:** Desenvolver um aplicativo dedicado para uma melhor experiência do usuário.
- **Hardware Customizado:** Projetar uma PCB dedicada para um produto final compacto.

---

## 👨‍💻 Autor

- **Michel L. Sampaio**

## 📜 Licença

Este projeto está sob a licença MIT. Veja o arquivo `LICENSE` para mais detalhes.