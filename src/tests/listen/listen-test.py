import requests

# file1 = open("basic.conf")
with open('/home/bruno/Documents/42SP/12.Webserv/Webserv-git/src/tests/listen/basic.conf', 'r') as configFile:
	if 'listen' in configFile.read():
		listen = configFile.read().rsplit("=")
		print(listen)

	# for lines in f:
	# 	>>> 'listen' in lines

# url = 'localhost::1026'

# response = requests.options(url)
# if 	response.status_code == 200:
# 	print("Sucesso - listen ok")
# else:
# 	print("Erro - listen falhou")