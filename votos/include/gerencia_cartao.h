#ifndef gerencia_cartao_h
#define gerencia_cartao_h

#include <FS.h>
#include "candidato.h"

void inicializar_cartao();
void ler_arquivo(fs::FS &fs, const char * path, int *quant, Candidato candidatos[]);
void escrever_arquivo (fs::FS &fs, const char * path, const char * message);
void limpar_arquivo(fs::FS &fs, const char * path);

#endif