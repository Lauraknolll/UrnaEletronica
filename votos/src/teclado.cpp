#include "teclado.h"

const int qtdLinhas = 4;
const int qtdColunas = 4;

char matriz_teclas[qtdLinhas][qtdColunas] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
//pino 1 conectado no 13, 12, 14, 27, 26, 25, 33, 32 no pino 8
byte PinosqtdLinhas[qtdLinhas] = {32, 33, 25, 26}; //conecta as linhas aos pinouts do teclado
byte PinosqtdColunas[qtdColunas] = {27, 14, 12, 13}; //conecta as colunas aos pinouts do teclado

Keypad meuteclado = Keypad( makeKeymap(matriz_teclas), PinosqtdLinhas, PinosqtdColunas, qtdLinhas, qtdColunas);

int ler_teclas()
{
    char tecla_pressionada = meuteclado.getKey();
    static String numero = "";
    if (tecla_pressionada)
    {
        switch (tecla_pressionada)
        {
            //quer corrigir o voto
            case '*':
            {
                //se já apertou algum número
                if(numero.length() > 0)
                {
                    numero.remove(numero.length() - 1);
                    Serial.printf("numero : %s\n", numero);
                }
                break;
            }
            //confirmou o voto
            case '#':
            {
                int voto;
                voto = numero.toInt();
                numero = "";
                Serial.printf("Voto confirmado : %d\n", voto);
                return voto;
            }
            //votando o primeiro ou segundo número
            default:
            {
                if (tecla_pressionada >= '0' && tecla_pressionada <= '9')
                {
                    numero += tecla_pressionada;
                    Serial.printf("numero : %s\n", numero);
                }
                break;
            }
        }
    }

    return -1;
}