import datetime
import subprocess
import time
import sys
import os

if len(sys.argv) != 2:
    print(f"Uso: python {sys.argv[0]} <n>. Onde n e o numero de processos a serem rodados.\n")
    sys.exit(1)

executable = "client.py"  # Replace with the actual path to your C executable
file_path = './results.txt'  # Replace with the actual path to your text file
n = int(sys.argv[1])  # Number of times to execute the executable
r = 10

def line_piece_to_string():
    #  Read the specified line from the file
    with open(file_path, 'r') as file:
        lines = file.readlines()
        first_line_piece = lines[0].strip()
        last_line_piece = lines[(n * r) - 1].strip()

    # Convert the line into a character vector
    first_line_vector = list(first_line_piece)
    first_ts_vector = [first_line_vector[len(first_line_vector) - 17], first_line_vector[len(first_line_vector) - 16], first_line_vector[len(first_line_vector) - 15], first_line_vector[len(first_line_vector) - 14], first_line_vector[len(first_line_vector) - 13], first_line_vector[len(first_line_vector) - 12], first_line_vector[len(first_line_vector) - 11], first_line_vector[len(first_line_vector) - 10], first_line_vector[len(first_line_vector) - 9], first_line_vector[len(first_line_vector) - 8], first_line_vector[len(first_line_vector) - 7], first_line_vector[len(first_line_vector) - 6]]
    first_ts_string = "".join(first_ts_vector)

    last_line_vector = list(last_line_piece)
    last_ts_vector = [last_line_vector[len(last_line_vector) - 17], last_line_vector[len(last_line_vector) - 16], last_line_vector[len(last_line_vector) - 15], last_line_vector[len(last_line_vector) - 14], last_line_vector[len(last_line_vector) - 13], last_line_vector[len(last_line_vector) - 12], last_line_vector[len(last_line_vector) - 11], last_line_vector[len(last_line_vector) - 10], last_line_vector[len(last_line_vector) - 9], last_line_vector[len(last_line_vector) - 8], last_line_vector[len(last_line_vector) - 7], last_line_vector[len(last_line_vector) - 6]]
    last_ts_string = "".join(last_ts_vector)

    return [first_ts_string, last_ts_string]


def timestamp_to_milliseconds(timestamp):
    # Parse the timestamp string into datetime object
    time_obj = datetime.datetime.strptime(timestamp, '%H:%M:%S:%f')

    # Calculate the milliseconds since the Unix epoch
    epoch = datetime.datetime.utcfromtimestamp(0)
    milliseconds = (time_obj - epoch).total_seconds() * 1000

    return int(milliseconds)


# Execute the C executable N times sequentially
processes = []
for _ in range(n):
    try:
        process = subprocess.Popen(["python3", executable], )
    except subprocess.CalledProcessError as e:
        print(f"Error executing {executable}: {e}")

    print(f"process no. {_ + 1} running\n")
    processes.append(process)


retry_count = 0
max_retries = 5
retry_interval = 3

while (True):
    while retry_count < max_retries:
        try:
            with open(file_path, "r") as file:
                lines = file.readlines()
                break  # Exit the loop if the file is found
        except FileNotFoundError:
            print("File not found. Retrying...")
            retry_count += 1
            time.sleep(retry_interval)
    else:
        print("Maximum number of retries reached. File not found.")
        
    if (len(lines) == n*r):
        print(f"Já tem {n*r} linhas no arquivo. Calculemos o tempo decorrido entre a primeira e a segunda linhas:\n")

        break


first_ts, last_ts = line_piece_to_string()

first_line_milliseconds = timestamp_to_milliseconds(first_ts)
last_line_milliseconds = timestamp_to_milliseconds(last_ts)
milli_diff = last_line_milliseconds - first_line_milliseconds 

print(f"diff in milliseconds: {milli_diff}")
print(f"diff in seconds: {milli_diff / 1000}")



