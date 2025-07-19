# Fase 2: Núcleo de Monitoramento de Áudio

## Objetivo da Fase
Implementar a lógica central do sistema: a detecção de som. O objetivo era criar um firmware capaz de medir a intensidade do ruído ambiente e acionar um alerta simples quando um limiar fosse excedido.

## Funcionalidades Implementadas
- **Aquisição de Áudio:** Leitura contínua de amostras do microfone via ADC.
- **Cálculo de RMS:** Implementação do algoritmo Root Mean Square para obter uma métrica estável do nível de ruído.
- **Lógica de Alerta Simples:** Comparação do valor RMS com um limiar (`threshold`) pré-definido no código.
- **Feedback Visual:** O LED RGB da placa acende em vermelho quando o alerta é ativado e em verde durante o monitoramento normal.

## Como Executar
1. Compile e carregue o firmware desta fase na Pico W.
2. Abra um monitor serial para visualizar o status.
3. Faça um som alto perto do microfone para disparar o alerta.

## Saída Esperada no Console
O console exibirá o nível de ruído RMS calculado a cada ciclo e o status do alerta.

```
--- SMAIAS: FASE 2 - NUCLEO DE MONITORAMENTO ---
Sistema ativo. Monitorando o nivel de ruido...
Limiar de Alerta (Threshold) configurado para: 150.0

Nivel de Ruido (RMS): 17.03   | Alerta: Nao
Nivel de Ruido (RMS): 20.86   | Alerta: Nao
Nivel de Ruido (RMS): 165.71  | Alerta: SIM!
Nivel de Ruido (RMS): 172.30  | Alerta: SIM!
Nivel de Ruido (RMS): 25.19   | Alerta: Nao
...
```

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
