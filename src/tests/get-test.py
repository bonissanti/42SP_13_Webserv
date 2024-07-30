import requests

def getRequest(port, file):
	server_name = 'http://localhost:' + port + file
	url = server_name

	try:
		response = requests.get(url)
	
		assert response.status_code == 200, f"Expected 200 OK, got {response.status_code}"

		expectedContent = "alguma coisa"
		assert response.text == expectedContent, f"Expected content '{expectedContent}', got '{response.text}'"		

		print("Teste GET passou com sucesso!")
	except requests.exceptions.RequestException as e:
		print(f"Erro na requisição {e}")
	

def readFile():
	with open('/home/bruno/Documents/42SP/12.Webserv/Webserv-git/config/basic.conf', 'r') as file:
		for line in file:
			if 'listen' in line:
				key, value = line.split('=')
				value = value.strip()