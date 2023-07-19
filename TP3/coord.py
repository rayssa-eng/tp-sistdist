import threading
import socket
import os
from collections import deque

REQUEST = 1
GRANT = 2
RELEASE = 3

HOST = "127.0.0.1"
PORT = 8080

client_queue = deque([])
client_dict = {}

queue_lock = threading.Lock()
dict_lock = threading.Lock()

def write_on_log(r_or_s, msg, pid):
    with open('log_coord.txt', 'a') as fp:
        log_message = f'[{r_or_s}] ' + f'{msg}' + f' | {pid}\n'
        fp.write(log_message)

class Client:
    def __init__(self, conn, pid):
        self.conn = conn
        self.pid = pid


def acquire(client):
    queue_lock.acquire()

    if (len(client_queue) == 0):
        client_queue.append(client)
        queue_lock.release()
        client.conn.sendall(str(GRANT).encode('utf-8'))
        write_on_log('S', 'Grant', client.pid)
    else:
        client_queue.append(client) 
        queue_lock.release()

def release(client):
    queue_lock.acquire()

    client_queue.popleft()
    if (len(client_queue) != 0):
        # client agora eh a nova cabeca
        client = client_queue[0] 
        queue_lock.release()

        client.conn.sendall(str(GRANT).encode('utf-8'))
        write_on_log('S', 'Grant', client.pid)
    else:
        queue_lock.release()
        pass


def handle_client_msg(conn):
    with conn:
        while True:
            client_msg = conn.recv(10).decode('utf-8')
            if len(client_msg) == 0:
                return None
            msg_type = int(client_msg[0])

            client_pid = int(client_msg[1:].lstrip())
            client = Client(conn, client_pid)

            if msg_type == REQUEST:
                write_on_log('R', 'Request', client.pid)    

                acquire(client)
                
                client_msg = conn.recv(10).decode('utf-8')
                msg_type = int(client_msg[0])

                client_pid = int(client_msg[1:].lstrip())
                client = Client(conn, client_pid)

                if msg_type == RELEASE:
                    write_on_log('R', 'Release', client.pid)    

                    release(client)

                    dict_lock.acquire()
                    if client.pid not in client_dict:
                        client_dict[client.pid] = 0
                    client_dict[client.pid] += 1
                    dict_lock.release()
                    continue
                else:
                    print(msg_type)
                    raise Exception

            else:
                raise Exception
                
def shell():
    while True:
        args = input('shell> ' ).strip().split()

        if (len(args) != 1):
            print("Erro. Digite de novo.")
            continue

        try:
            code = int(args[0])
        except:
            print("Erro. Digite de novo")
            continue
        
        if code == 1:
            print("Processos em fila: ")
            queue_lock.acquire()
            for client in client_queue:
                print(f"{client.pid} | ")
            queue_lock.release()
        elif code == 2:
            client_pid = int(input("De qual processo voce quer saber? Insira o PID: "))
             
            dict_lock.acquire()
            if client_pid in client_dict:
                print(f"{client_pid} foi atendide {client_dict[client_pid]} vezes")
            else:
                print("Nao conheco esse processo :/")
            dict_lock.release()

        elif code == 3:
            print("Encerrando... Ate a proxima!")
            os.kill(os.getpid(), 9)
           


def create_thread(conn):
    t = threading.Thread(target=handle_client_msg, args=(conn,))
    t.start()

def main():
    with open('results.txt', 'w') as fp:
        fp.write('')

    shell_thread = threading.Thread(target=shell)
    shell_thread.start()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as skt:
        skt.bind((HOST, PORT))
        skt.listen()

        while True:
            conn, addr = skt.accept()
            create_thread(conn)

main()
