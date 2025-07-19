#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H

/**
 * @brief Inicializa os recursos de hardware necessários para o processamento de áudio.
 */
void audio_init(void);

/**
 * @brief Lança o loop de processamento de áudio no Core 1.
 * @details Esta função inicia o segundo núcleo do RP2040, que ficará
 *          dedicado a calcular o nível de ruído (RMS) e enviar os
 *          resultados para o Core 0 via FIFO.
 */
void audio_launch_on_core1(void);

#endif