#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include "gerencia_display.h"

#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   12
#define TFT_DC   14
#define TFT_RST  4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#include <TJpg_Decoder.h>

bool tft_output(int16_t x, int16_t y,
                uint16_t w, uint16_t h,
                uint16_t *bitmap)
{
    tft.startWrite();
    tft.setAddrWindow(x, y, w, h);
    tft.writePixels(bitmap, w * h, true);
    tft.endWrite();

    return true;
}

void inicializa_display()
{
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(2);
    limpa_tela();

    escreve_info("Iniciando sistema.");

    delay(5000);

    escreve_info("Digite seu voto");
}

void escreve_info(String informacao)
{
    limpa_tela();

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(10, 60);
    tft.print(informacao);
}

void escreve_candidato(int num, String nome)
{

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 20);
    tft.print(num);
    tft.setTextSize(1);
    tft.setCursor(40, 20);
    tft.print(nome);
}

void escreve_digitado(String num)
{
    limpa_tela();

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(2);
    tft.setCursor(10, 20);
    tft.print(num);
}

void escreve_modoAdmin()
{
    limpa_tela();

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);
    tft.setCursor(10, 20);
    tft.print("Modo Administrador");

    tft.setCursor(10, 40);
    tft.print("1 - Iniciar nova\n    eleicao.");

    tft.setCursor(10, 70);
    tft.print("2 - Continuar \n    ultima eleicao.");

    tft.setCursor(10, 100);
    tft.print("3 - Encerrar \n    eleicao atual.");

    tft.setCursor(10, 130);
    tft.print("4 - Sair.");
}

void escreve_senha(String mascara)
{
    limpa_tela();

    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(1);

    tft.setCursor(10, 60);
    tft.print("Digite a senha:");

    int x = (128 - mascara.length() * 6) / 2;
    tft.setCursor(x, 100);
    tft.print(mascara);
}

void escreve_foto(String path)
{
    tft.fillScreen(ST77XX_BLACK);
    TJpgDec.setCallback(tft_output);
    TJpgDec.setJpgScale(1);
    JRESULT res = TJpgDec.drawSdJpg(10, 40, path.c_str());
}

void limpa_tela()
{
    tft.fillScreen(ST77XX_BLACK);
}