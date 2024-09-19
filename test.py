import socket
import time

def send_chunked_request():
    host = 'localhost'
    port = 1027
    request_headers = (
        "POST /upload HTTP/1.1\r\n"
        "Host: localhost:1027\r\n"
        "Transfer-Encoding: chunked\r\n"
        "Content-Type: text/plain\r\n"
        "\r\n"
    )

    chunks = [
        "4\r\nWiki\r\n",
        "5\r\npedia\r\n",
        "E\r\n in\r\n chunks.\r\n",
        "0\r\n\r\n"
    ]

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((host, port))
        s.sendall(request_headers.encode())

        for chunk in chunks:
            s.sendall(chunk.encode())
            time.sleep(1)  # Simulate delay between chunks

        response = s.recv(4096)
        print(response.decode())

if __name__ == "__main__":
    send_chunked_request()