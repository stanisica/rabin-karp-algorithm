import random
import os

def generate_random_sequence():
    sequences = ["R1", "L1", "X", "O", "Y"]
    sequence_length = random.randint(50, 50000)
    return ''.join(random.choice(sequences) for _ in range(sequence_length))

def generate_file(file_name, num_sequences, size_mb):
    target_size = size_mb * 1024 * 1024
    with open(file_name, 'w') as file:
        while os.path.getsize(file_name) < target_size:
            sequence = generate_random_sequence()
            file.write(sequence + '\n')
            file.flush()  
        file.truncate(target_size)

if __name__ == "__main__":
    print("Generating sequences...")
    
    file_name = "controller_input.txt"
    num_sequences = 1000
    size_mb = 500
    generate_file(file_name, num_sequences, size_mb)
    
    print("Sequences generated.")
