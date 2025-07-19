# Fase 5: Sistema Integrado e Otimização Dual-Core

![Status da Fase](https://img.shields.io/badge/status-versão%20final-blue)

## Objetivo da Fase

Esta é a fase de conclusão e otimização do protótipo SMAIV. O objetivo foi duplo:
1.  **Integrar todas as funcionalidades** em um sistema coeso, incluindo os alertas multimodais completos e lógica de alarme.
2.  **Refatorar e otimizar o firmware**, migrando o código para uma arquitetura de software modular e aproveitando a **arquitetura dual-core do RP2040** para maximizar o desempenho e a responsividade.

---

## Funcionalidades e Otimizações Finais

Esta versão representa o protótipo completo, com todas as funcionalidades integradas e otimizadas:

1.  **Alertas Locais Multimodais:** O sistema agora aciona um conjunto completo de atuadores (LED RGB, Buzzer e Matriz de LEDs WS2182B) para fornecer um alerta local inconfundível.

2.  **Lógica de Alarme com Travamento (*Latching*):** O dispositivo se comporta como um sistema de segurança real. Uma vez disparado, o alarme permanece ativo até ser reconhecido e silenciado manualmente pelo usuário (pressionando o Botão A), garantindo que nenhum evento seja perdido.

3.  **Período de Silêncio Pós-Alerta:** Após um alarme ser silenciado, o sistema entra em um período de "graça" de 5 segundos, durante o qual novos alertas são suprimidos. Isso evita reativações indesejadas causadas pelo mesmo evento sonoro.

---

## Arquitetura Final: Software Modular e Dual-Core

A mudança mais significativa nesta fase foi a transição para uma arquitetura de software profissional, com duas melhorias principais:

### 1. Refatoração para Módulos

O código monolítico das fases anteriores foi decomposto em módulos lógicos, cada um com uma responsabilidade única. O `main.c` agora atua como um **orquestrador**, coordenando as chamadas para os módulos:
- `modules/audio_processing/`
- `modules/ui_manager/`
- `modules/local_alerts/`
- `modules/mqtt_comm/`

### 2. Otimização com Processamento Paralelo (Dual-Core)

Para garantir a máxima eficiência e responsividade, as tarefas do sistema foram divididas entre os dois núcleos do RP2040:
- **Core 1 (Co-processador de Sinal):** Foi dedicado exclusivamente à tarefa computacionalmente intensiva de **aquisição de áudio e cálculo de RMS**. Ele opera em um loop contínuo, enviando os resultados para o Core 0.
- **Core 0 (Núcleo Principal):** Gerencia todas as outras tarefas: **lógica de estado, interface com o usuário, conectividade de rede e controle de atuadores**.

A comunicação entre os núcleos é realizada de forma segura através da **FIFO inter-core**, um mecanismo *thread-safe* que previne condições de corrida e garante a integridade dos dados.

---

O desenvolvimento do projeto está sendo realizado em fases. O status atual é:

- [X] **Fase 1: Testes de Hardware (Concluída)**
  - *Descrição:* Todos os periféricos da placa BitDogLab (LEDs, botões, joystick, buzzer, OLED, microfone) foram testados e validados de forma interativa.

- [X] **Fase 2: Núcleo de Monitoramento (Concluída)**
  - *Descrição:* O sistema agora lê continuamente o microfone, calcula o nível de ruído ambiente (RMS) e o compara com um limiar pré-definido. Um alerta visual (LED RGB) é ativado quando o limiar é excedido.

- [x] **Fase 3: Interface do Usuário (UI)**
  - *Descrição:* Desenvolvimento da interface no display OLED para visualização de status e configuração do limiar de sensibilidade através do joystick.

- [x] **Fase 4: Conectividade MQTT**
  - *Descrição:* Implementação da conexão Wi-Fi e envio de alertas remotos via protocolo MQTT.

- [x] **Fase 5: Integração Final e Refatoração**
  - *Descrição:* Combinação de todos os módulos (incluindo buzzer e matriz de LEDs nos alertas) e refatoração do código para uma arquitetura modular e implementação de Dual-Core.

---
_**Status:** Concluída. Protótipo finalizado e refatorado._

## Próximos Passos e Evolução

A arquitetura atual, embora eficiente, ainda é baseada em um loop "bare-metal". A próxima grande evolução do projeto seria a migração para um sistema operacional de tempo real (RTOS), que abriria caminho para funcionalidades ainda mais complexas e robustas.

### Versão 2.0: Migração para FreeRTOS

A adoção do **FreeRTOS** permitiria refatorar a arquitetura para um modelo baseado em tarefas com prioridades distintas:
- **Tarefa de Áudio (Alta Prioridade):** Substituiria a lógica do Core 1, garantindo a execução determinística do processamento de sinal.
- **Tarefa de Rede (Média Prioridade):** Gerenciaria a conexão MQTT e futuras comunicações, como um servidor web.
- **Tarefa de UI (Baixa Prioridade):** Manteria a interface do usuário responsiva sem interferir nas operações críticas.
- **Tarefa de Alertas (Alta Prioridade):** Gerenciaria os atuadores com latência mínima.

Essa migração não só aprimoraria a escalabilidade do software, permitindo a fácil adição de novos sensores e funcionalidades, mas também introduziria mecanismos de sincronização mais avançados, como semáforos e mutexes, para um gerenciamento de recursos ainda mais seguro.
