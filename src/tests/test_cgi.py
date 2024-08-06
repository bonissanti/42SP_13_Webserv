#!/usr/bin/ python3

import os
import urllib.parse
import sys
import pdb

def parseQueryString(query_string):
	return (dict(urllib.parse.parse_qsl(query_string)))

def	parsePostData():
	content_length = int(os.environ.get('CONTENT_LENGTH', 0))
	post_data = sys.stdin.read(content_length)
	return (dict(urllib.parse.parse_qsl(post_data)))
	
request_method = os.environ.get('REQUEST_METHOD', 'GET')

form_data = {} # dicionário de parâmetros

if request_method == 'GET':
	query_string = os.environ.get('QUERY_STRING', '')
	form_data = parseQueryString(query_string)
elif request_method == 'POST':
	form_data = parsePostData()
	
username = form_data.get("username", "Random")
emailAddress = form_data.get("emailaddress", "random@example.com")

print("Content-type: text/html\r\n\r\n")
print("<html>")
print("<head>")
print("<title>Unprotected login page</title>")
print("</head>")
print("<body>")
print("<h3>This is HTML's Body Section</h3>")
print("<p>Username: {}</p>".format(username))
print("<p>Email Address: {}</p>".format(emailAddress))
print("</body>")
print("</html>")