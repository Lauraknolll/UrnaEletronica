#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include <Arduino.h>
#include "gerencia_cartao.h"

#define SCK  18
#define MISO  19
#define MOSI  23
#define CS  5


//SPIClass spi = SPIClass(VSPI);

void inicializar_cartao ()
{

    SPI.begin(SCK, MISO, MOSI);

    //tenta comunicação no pino CS
    if(!SD.begin(CS))
    {
        Serial.println("Falha no SD");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE)
    {
        Serial.println("Nenhum cartão detectado");
        return;
    }

    Serial.println("SD inicializado com sucesso"); 
}

void ler_arquivo(fs::FS &fs, const char * path, int *quant, Candidato candidatos[])
{
  Serial.printf("Lendo arquivo: %s\n", path);

  File file = fs.open(path);
  if(!file)
  {
    Serial.println("Falha ao abrir o arquivo para leitura.");
    return;
  }

  while(file.available())
  {
    //crio um candidato
    Candidato c;
    String caractere_lido;
    //leio até o primeiro ponto e vírgula que é o número
    caractere_lido = file.readStringUntil(';');
    c.numero = caractere_lido.toInt();

    //leio até o final da linha que é o nome
    caractere_lido = file.readStringUntil('\n');
    //remove os espaços \r e \n
    caractere_lido.trim();
    c.nome = caractere_lido;
    c.quantidade_votos = 0;

    candidatos[*quant] = c;
    (*quant)++;
  }
  file.close();
}

void escrever_arquivo (fs::FS &fs, const char * path, const char * message)
{
  Serial.printf("Escrevendo no arquivo: %s\n", path);

  File file = fs.open(path, FILE_APPEND);

  if(!file)
  {
    Serial.println("Falha em abrir arquivo para escrita.");
    return;
  }

  if(file.println(message))
  {
    Serial.println("Escrita bem sucedida.");
  }   
  else 
  {
    Serial.println("Falha ao escrever.");
  }
  file.close();
}




