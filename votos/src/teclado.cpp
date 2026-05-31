#include "teclado.h"

const int qtdLinhas = 4;
const int qtdColunas = 3;

char matriz_teclas[qtdLinhas][qtdColunas] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
//pino 1 conectado no 13, 12, 14, 27, 26, 25, 33, 32 no pino 8 
//teclado 4 colunas
//byte PinosqtdLinhas[qtdLinhas] = {32, 33, 25, 26}; //conecta as linhas aos pinouts do teclado
//byte PinosqtdColunas[qtdColunas] = {27, 14, 12, 13}; //conecta as colunas aos pinouts do teclado
//teclado 3 colunas
byte PinosqtdLinhas[qtdLinhas] = {33, 32, 27, 26}; //conecta as linhas aos pinouts do teclado
byte PinosqtdColunas[qtdColunas] = {25, 21, 22}; //conecta as colunas aos pinouts do teclado

Keypad meuteclado = Keypad( makeKeymap(matriz_teclas), PinosqtdLinhas, PinosqtdColunas, qtdLinhas, qtdColunas);

char ler_teclas()
{
    char tecla_pressionada = meuteclado.getKey();
    if (tecla_pressionada)
    {
        return tecla_pressionada;
    }
    
    return 'F';
}