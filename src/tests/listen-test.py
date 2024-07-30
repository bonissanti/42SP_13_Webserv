import socket
import requests

RED 	= "\033[0;31m"
GREEN	= "\033[0;32m"
YELLOW	= "\033[0;33m"
RESET 	= "\033[0m"

def	checkServer(port):
	tcpSocket = socket.socket()
	server_name = '127.0.0.1'
	print(f"{YELLOW}Attempting to connect to 'localhost' on port %s{RESET}" % port)

	try:
		tcpSocket.connect((server_name, int(port)))
		print(f"{GREEN} Connected to 'localhost' on port %s{RESET}" % port)
		return (True)

	except socket.error as e:
		print(f"{RED}Connection to 'localhost' on port %s failed{RESET}" % port)
		return (False)
	
	finally:
		tcpSocket.close()

def readFile():
	with open('/home/bruno/Documents/42SP/12.Webserv/Webserv-git/config/basic.conf', 'r') as file:
		for line in file:
			if 'listen' in line:
				key, value = line.split('=')
				value = value.strip()
				checkServer(value)
	