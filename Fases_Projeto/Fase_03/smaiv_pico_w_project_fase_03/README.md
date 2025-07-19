# Fase 3: Interface com o Usuário (UI)

## Objetivo da Fase
Tornar o dispositivo autônomo, permitindo que o usuário visualize informações e configure parâmetros diretamente no hardware, sem a necessidade de um computador.

## Como Executar
1. Compile e carregue o firmware desta fase.
2. O dispositivo iniciará na tela de monitoramento.
3. Pressione o botão do joystick para entrar no modo de ajuste.
4. Mova o joystick para cima/baixo para alterar o limiar.
5. Pressione o Botão A para salvar.

## Saída Esperada (Visual)
- O display OLED mostrará as informações descritas, com a interface respondendo às ações do usuário. A saída do console confirmará o novo limiar salvo.

### Funcionalidades Implementadas

1.  **Integração do Display OLED:**
    *   O display OLED (baseado no controlador SSD1306) foi inicializado e configurado para comunicação via I2C.
    *   Foi utilizada a biblioteca `pico-ssd1306` para abstrair as operações de desenho.

2.  **Visualização de Status em Tempo Real:**
    *   A tela principal exibe continuamente o nome do projeto ("SMAIV").
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

- [X] **Fase 2: Núcleo de Monitoramento (Concluída)**
  - *Descrição:* O sistema agora lê continuamente o microfone, calcula o nível de ruído ambiente (RMS) e o compara com um limiar pré-definido. Um alerta visual (LED RGB) é ativado quando o limiar é excedido.

- [x] **Fase 3: Interface do Usuário (UI)**
  - *Descrição:* Desenvolvimento da interface no display OLED para visualização de status e configuração do limiar de sensibilidade através do joystick.

- [ ] **Fase 4: Conectividade MQTT**
  - *Descrição:* Implementação da conexão Wi-Fi e envio de alertas remotos via protocolo MQTT.

- [ ] **Fase 5: Integração Final e Refatoração**
  - *Descrição:* Combinação de todos os módulos (incluindo buzzer e matriz de LEDs nos alertas) e refatoração do código para uma arquitetura modular e limpa.
