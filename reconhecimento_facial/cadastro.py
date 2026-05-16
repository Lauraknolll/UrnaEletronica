import face_recognition
import sqlite3
import numpy as np

# 1. CONFIGURAÇÃO

dados = {
    "laura.jpg" : "laura",
    "bruna.jpg" : "bruna",
    "guilherme.jpg" : "guilherme"
}

# 2. CONECTAR AO BANCO

# cria um banco temporário já que não temos o nosso ainda
conn = sqlite3.connect("faces.db")
cursor = conn.cursor()

# criar tabela se não existir
cursor.execute("""
CREATE TABLE IF NOT EXISTS faces (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT,
    encoding BLOB
)
""")

# 3. PROCESSAR IMAGENS

for arquivo, nome in dados:
    print(f"Processando {nome}...")

    # converte a imagem pra numpy array
    img = face_recognition.load_image_file(arquivo)
    # gera um vetor pra cada rosto encontrado na imagem
    encodings = face_recognition.face_encodings(img)

    if len(encodings) == 0:
        print(f"Nenhum rosto encontrado em {arquivo}")
        continue

    #sabemos que só tem um rosto por foto
    encoding = encodings[0]

    # Converter numpy array pra bytes
    encoding_bytes = encoding.tobytes()

    # 4. SALVAR NO BANCO
    cursor.execute(
        "INSERT INTO faces (name, encoding) VALUES (?, ?)",
        (nome, encoding_bytes)
    )

    print(f"{nome} cadastrado!")

# 5. FINALIZAR

conn.commit()
conn.close()

print("Cadastro de todas as imagem disponíveis finalizado.")