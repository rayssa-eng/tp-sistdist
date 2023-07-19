import socket
import os
import time
from datetime import datetime

REQUEST = 1
GRANT = 2
RELEASE = 3

HOST = "127.0.0.1"
PORT = 8080

r = 3
k = 0

def write_on_results(pid):
    with open('results.txt', 'a') as fp:
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S:%f")
        fp.write(f'{pid} {timestamp}\n')

def padding(code, pid):
    return f"{code}{pid}".ljust(10, ' ')

def main():
    my_pid = os.getpid()
    request_message = padding(1, my_pid)
    release_message = padding(3, my_pid)
       
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as skt:
        skt.connect((HOST, PORT))

        for i in range(r):
            skt.sendall(request_message.encode('utf8'))
            print(f"{my_pid} enviou request")

            serv_ans = skt.recv(10).decode('utf8')

            if serv_ans:
                print(f"{my_pid} recebeu grant")

            write_on_results(my_pid)

            skt.sendall(release_message.encode('utf8'))
            print(f"{my_pid} enviou release")

            time.sleep(k)

        skt.close()
        exit()

main()



    

      