#!/usr/bin/env python3
import requests
import cgi

# URL da API pública
api_url = "http://api.weatherapi.com/v1/current.json"
api_key = "90c5a08f95c54ec48a4170335242908"

# Lê a localização enviada pelo formulário

form = cgi.FieldStorage()
location = form.getvalue("location", "São Paulo")


# Faz a requisição GET para a API
response = requests.get(f"{api_url}?key={api_key}&q={location}")

# Verifica se a requisição foi bem-sucedida
if response.status_code == 200:
    data = response.json()
    # Processa e exibe os dados da resposta
    print(f"<html><body><h1>Clima em {location}</h1>")
    print(f"<p>Temperatura: {data['current']['temp_c']}°C</p>")
    print(f"<p>Condição: {data['current']['condition']['text']}</p>")
    print("</body></html>")
else:
    print("<html><body><h1>Erro na requisição à API</h1></body></html>")