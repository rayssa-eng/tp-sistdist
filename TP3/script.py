import subprocess
import sys
# import numpy as np

if len(sys.argv) != 2:
    print(f"Uso: python {sys.argv[0]} <n>. Onde n e o numero de processos a serem rodados.\n")
    sys.exit(1)

n = sys.argv[1]

def run():
    cmd = f"./client"

    for i in n:
        subprocess.check_output(cmd, shell=True).decode("utf-8")
        
run()