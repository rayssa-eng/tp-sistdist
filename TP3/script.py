import subprocess
import sys

if len(sys.argv) != 2:
    print(f"Uso: python {sys.argv[0]} <n>. Onde n e o numero de processos a serem rodados.\n")
    sys.exit(1)

executable_path = "./client"  # Replace with the actual path to your C executable
N = int(sys.argv[1])  # Number of times to execute the executable

# Execute the C executable N times sequentially
processes = []
for _ in range(N):
    process = subprocess.Popen([executable_path])
    print(f"process no. {_ + 1} running")
    processes.append(process)

print(f"All {N} processes are running.")
