import face_recognition
import cv2
import sqlite3
import numpy as np

import serial
import time

from RPLCD.i2c import CharLCD
from time import sleep

lcd = CharLCD(
    i2c_expander='PCF8574',
    address=0x27,
    port=1,
    cols=16,
    rows=2
)

lcd.clear()

def aguarda_votacao():
    resposta = None
    while True:
        resposta = ser.readline().decode().strip()
        if resposta == "Votação realizada":
            print("ESP informou FIM FIM  FIM FIM FIM FIM")
            break

ser = serial.Serial(
    '/dev/serial0',
    115200,
    timeout=1
)
contador_frames = 0
autenticados = set()

# 1. LER DO BANCO

conn = sqlite3.connect("faces.db")
cursor = conn.cursor()

# lê todos os nomes disponíveis no banco 
cursor.execute("SELECT name, encoding FROM faces")
rows = cursor.fetchall()

known_names = []
known_encodings = []
# nome = "Joaquino"
# print(f"{nome} autenticada(o)!")
# # mensagem para a pessoa poder votar
# mensagem = "Pode votar\n"

# ser.write(mensagem.encode())
# time.sleep(1)
# resposta = ser.readline().decode().strip()

# if resposta:
#     print(f"Usuário {nome} Autenticado", resposta)
#     print(f"Usuário {nome} pode iniciar votação", resposta)

# resposta = None

# while True:
#     resposta = ser.readline().decode().strip()
#     if resposta == "Votação realizada":
#         print("ESP informou FIM FIM  FIM FIM FIM FIM")
#         break

for name, encoding_bytes in rows:
    # Converter bytes pra numpy array
    encoding = np.frombuffer(encoding_bytes, dtype=np.float64)

    # coloca os nomes e encodings em duas listas
    known_names.append(name)
    known_encodings.append(encoding)

conn.close()

print("Base carregada para ", len(known_names), "pessoas")

# 2. WEBCAM

# abre a fonte principal de vídeo 0
cap = cv2.VideoCapture(0)
# cap.set(cv2.CAP_PROP_FRAME_WIDTH, 1920)
# cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 1080)

while True:
    print("O PROCESSO COMECOU DE VOLTAAAAA")
    ret, frame = cap.read()

    #se falhou em capturar a imagem
    if not ret: 
        break
    
    # corrige o formato da cor de BGR pra RGB (do openCV pro face)
    rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    rgb = rgb.copy()

    # retorna as coordenadas de cada rosto presente na imagem
    face_locations = face_recognition.face_locations(rgb)
    # extrai o encoding do rosto que tá na câmera
    face_encodings = face_recognition.face_encodings(rgb, face_locations)

    for (top, right, bottom, left), face_enc in zip(face_locations, face_encodings):
        #enquadramento
        cx_imagem = 320
        cy_imagem = 240

        cx_face = (left + right) // 2
        cy_face = (top + bottom) // 2

        erro_x = cx_face - 320
        erro_y = cy_face - 240

        if erro_x < -165:
            print("Mova para a esquerda")
            lcd.clear()
            lcd.write_string("Mova para a esquerda")
            
        elif erro_x > 165:
            print("Mova para a direita")
            lcd.clear()
            lcd.write_string("Mova para a direita")

        

        else:
            print("Centralizado")
            lcd.clear()
            lcd.write_string("Centralizado")



        # valores padrão se não conhecer a pessoa 
        nome = "DESCONHECIDO"
        cor = (0, 0, 255)
        best_distance = 1.0

        # se tiver rostos cadastrados
        if len(known_encodings) > 0:

            # 3. COMPARAÇÃO
            # calcula a distância entre o rosto da câmera e TODOS os rostos conhecidos
            distances = face_recognition.face_distance(known_encodings, face_enc)

            # pega o menor valor da distância (o mais parecido)
            best_index = np.argmin(distances)
            best_distance = distances[best_index]

            # se for menor que 0.5 considera que é a pessoa
            if best_distance < 0.45:
                nome = known_names[best_index]
                cor = (0, 255, 0)

        # 4. DESENHAR RESULTADO
        # desenha o quadrado no rosto da câmera
        cv2.rectangle(frame, (left, top), (right, bottom), cor, 2)
        # escreve o nome que reconheceu
        cv2.putText(frame, f"{nome} ({best_distance:.2f})",
                    (left, top - 10),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.7, cor, 2)

        if nome != "DESCONHECIDO":
            contador_frames += 1
        else:
            contador_frames = 0
        
        if (contador_frames > 20) and nome not in autenticados:
            autenticados.add(nome)
            print(f"{nome} autenticada(o)!")
            #   mensagem para a pessoa poder votar
            mensagem = "Pode votar\n"
            ser.write(mensagem.encode())
            time.sleep(1)
            resposta = ser.readline().decode().strip()       
            if resposta:
                print(f"Usuário {nome} Autenticado", resposta)
                print(f"Usuário {nome} pode iniciar votação", resposta)
                lcd.clear()
                lcd.write_string(f"Autenticada(o)!")
                lcd.cursor_pos = (1, 0)
                lcd.write_string("Pode votar!")
                aguarda_votacao()
                
        
    cv2.resize(frame, (640, 480))
    cv2.imshow("Reconhecimento Facial", frame)

    # ESC ou fechar janela
    if cv2.waitKey(1) == 27 or cv2.getWindowProperty("Reconhecimento Facial", cv2.WND_PROP_VISIBLE) < 1:
        print(autenticados)
        break

#desliga a câmera
cap.release()
cv2.destroyAllWindows()