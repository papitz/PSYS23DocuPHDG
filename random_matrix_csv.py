import csv
import random

def generate_random_csv(filename, rows, cols, zero_fraction):
    with open(filename, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile, delimiter=',', quoting=csv.QUOTE_MINIMAL)
        
        for _ in range(rows):
            row = []
            for _ in range(cols):
                if random.random() < zero_fraction:
                    row.append(0.0)
                else:
                    row.append(random.uniform(0.0, 15000.0))
            csvwriter.writerow(row)

if __name__ == "__main__":
    filename = "random_matrix.csv"
    rows = 100
    cols = 100
    zero_fraction = 0.99

    generate_random_csv(filename, rows, cols, zero_fraction)
    print(f"CSV file '{filename}' with random values has been generated.")
