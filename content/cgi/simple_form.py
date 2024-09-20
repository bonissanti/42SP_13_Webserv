#!/usr/bin/python3

import os
from pathlib import Path

base_dir = Path(__file__).resolve().parent.parent
not_allowed_page_path = base_dir / "page-errors" / "405.html"
database_path = base_dir / "database" / "user_emails.csv"
error_page_path = base_dir / "page-errors" / "400.html"
internal_error_page_path = base_dir / "page-errors" / "500.html"

if (os.environ.get("REQUEST_METHOD", None) != "POST"):
    with open(not_allowed_page_path, "r") as f:
        print(f.read())

else:
    if not os.path.exists(database_path):
        with open(database_path, "w") as file:
            file.write("")
    os.chmod(database_path, 0o777)
    content = os.environ.get("QUERY_STRING", None)
    if content is None:
        with open(error_page_path, "r") as f:
            print(f.read())

    try:
        email = content.split("&")[0].split("=")[1]
        name = content.split("&")[1].split("=")[1]
        phone = content.split("&")[2].split("=")[1]
        message = content.split("&")[3].split("=")[1]

        print("Content-Type: text/html")
        print()

        print("<!DOCTYPE html>")
        print("<html lang='en'>")
        print("<head>")
        print("    <meta charset='UTF-8'>")
        print("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>")
        print("    <title>Hello from CGI!</title>")
        print("    <link href='https://fonts.googleapis.com/css?family=Montserrat:700,900' rel='stylesheet'>")
        print("    <style>")
        print("        body {")
        print("            background-color: #333333FF;")
        print("            font-family: Montserrat, sans-serif;")
        print("            color: #9435d3; /* Cute purple color */")
        print("            margin: 0;")
        print("            padding: 20px;")
        print("            text-align: center; /* Center align the content */")
        print("        }")
        print("        h1, h2 {")
        print("            color: rgb(255, 255, 255);")
        print("        }")
        print("        #content {")
        print("            background-color: #CCCCCCFF;")
        print("            font-size: 14px;")
        print("            color: #333333FF;")
        print("            text-align: center;")
        print("            padding: 20px;")
        print("            width: 100%;")
        print("            position: absolute;")
        print("            bottom: 0;")
        print("            left: 0;")
        print("            margin-top: 20px;")
        print("        }")
        print("        a {")
        print("            color: #777777FF;")
        print("        }")
        print("    </style>")
        print("</head>")
        print("<body>")
        print("<h1>Hello from CGI!</h1>")
        print("<p>Esse é um script CGI escrito em Python.</p>")
        print("<h2>Os dados enviados foram:</h2>")

        if name and email and phone:
            print(f"<p>Nome: {name}</p>")
            print(f"<p>Email: {email}</p>")
            print(f"<p>Telefone: {email}</p>")
            print(f"<p>Mensagem {message}</p>")
            with open(database_path, "a") as file:
                file.write(f"\n{name},{email}, {phone}, {message}")
                file.close()
        else:
            print("<p>Infelizmente não recebemos os dados que precisávamos :(</p>")

        print("<div id='content'>")
        print("    <p>Would you like to go back to the <a href='/' style='color:#777777FF;'>homepage</a>?</p>")
        print("    <p>------------------------------------------------</p>")
        print("    <p>Você gostaria de voltar para o <a href='/' style='color:#777777FF;'>início</a>?</p>")
        print("</div>")
        print("</body></html>")

    except Exception as e:
        with open(internal_error_page_path, "r") as f:
            print(f.read())
