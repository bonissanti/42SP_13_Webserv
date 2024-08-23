#!/usr/bin/env python3

import datetime
import os
from pathlib import Path

base_dir = Path(__file__).resolve().parent.parent
not_allowed_page_path = base_dir / "error_pages" / "405.html"
internal_error_page_path = base_dir / "error_pages" / "500.html"

if (os.environ.get("REQUEST_METHOD", None) != "GET"):
    with open(not_allowed_page_path, "r") as f:
        print(f.read())
else:
    try:
        print("Content-Type: text/html")
        print()

        print("<!DOCTYPE html>")
        print("<html lang='en'>")
        print("<head>")
        print("    <meta charset='UTF-8'>")
        print("    <meta name='viewport' content='width=device-width, initial-scale=1.0'>")
        print("    <title>Server Time and Info</title>")
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

        print("<h1>Current Time</h1>")
        print("<p>{}</p>".format(datetime.datetime.now()))

        print("<h2>Server Details</h2>")
        print("<p>Server Name: {}</p>".format(os.environ.get('SERVER_NAME', 'Unknown')))
        print("<p>Request Method: {}</p>".format(os.environ.get('REQUEST_METHOD', 'Unknown')))
        print("<p>Port Number: {}</p>".format(os.environ.get('SERVER_PORT', 'Unknown')))
        print("<p>Path: {}</p>".format(os.environ.get('PATH_TRANSLATED', 'Unknown')))
        print("<p>Content Length: {}</p>".format(os.environ.get('CONTENT_LENGTH', 'Unknown')))
        print("<p>Content Type: {}</p>".format(os.environ.get('CONTENT_TYPE', 'Unknown')))

        print("<div id='content'>")
        print("    <p>Would you like to go back to the <a href='/' style='color:#777777FF;'>homepage</a>?</p>")
        print("    <p>------------------------------------------------</p>")
        print("    <p>Você gostaria de voltar para o <a href='/' style='color:#777777FF;'>início</a>?</p>")
        print("</div>")
        print("</body></html>")

    except Exception as e:
        with open(internal_error_page_path, "r") as f:
            print(f.read())