#!/usr/bin/env python3

import datetime

print("<html>")
print("<head>")
print("<title>Data e Hora Atuais</title>")
print("<style>")
print("body { font-family: Arial, sans-serif; background-color: #f0f0f0; text-align: center; padding: 50px; }")
print("h1 { color: #333; }")
print("p { font-size: 1.2em; color: #666; }")
print(".container { background-color: #fff; padding: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0, 0, 0, 0.1); display: inline-block; }")
print("</style>")
print("</head>")
print("<body>")
print("<div class='container'>")
print("<h1>Data e Hora Atuais</h1>")

now = datetime.datetime.now()
print(f"<p>Data e Hora: {now.strftime('%Y-%m-%d %H:%M:%S')}</p>")

print("</div>")
print("</body>")
print("</html>")
