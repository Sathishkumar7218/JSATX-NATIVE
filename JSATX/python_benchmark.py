import pandas as pd
import numpy as np
import time

print("============================================")
print("          Pandas/Numpy — Large Data Benchmark")
print("============================================\n")

# Load DataFrame
print("Loading large CSV...")
load_start = time.time()
df = pd.read_csv("java/jsatx-java/large_sample.csv")
load_end = time.time()
load_time = (load_end - load_start) * 1000
print(f"Load time: {load_time:.2f} ms")

# DataFrame operations
print("\n--- DataFrame Operations ---")
df_start = time.time()

sum1 = df['value1'].sum()
mean1 = df['value1'].mean()
sum2 = df['value2'].sum()
mean2 = df['value2'].mean()
size = len(df)

df_end = time.time()
df_time = (df_end - df_start) * 1000

print(f"Rows: {size}")
print(f"value1 sum: {sum1}")
print(f"value1 mean: {mean1}")
print(f"value2 sum: {sum2}")
print(f"value2 mean: {mean2}")
print(f"DataFrame ops time: {df_time:.2f} ms")

# Matrix operations
print("\n--- Matrix Operations ---")
matrix_size = 500
print(f"Creating {matrix_size}x{matrix_size} matrices...")

matrix_start = time.time()

A = np.zeros((matrix_size, matrix_size))
B = np.zeros((matrix_size, matrix_size))

for i in range(matrix_size):
    for j in range(matrix_size):
        A[i, j] = np.sin(i + j)
        B[i, j] = np.cos(i - j)

C = np.dot(A, B)
matrix_sum = C.sum()
matrix_mean = C.mean()

matrix_end = time.time()
matrix_time = (matrix_end - matrix_start) * 1000

print(f"Matrix multiplication result sum: {matrix_sum}")
print(f"Matrix multiplication result mean: {matrix_mean}")
print(f"Matrix ops time: {matrix_time:.2f} ms")

total_time = load_time + df_time + matrix_time
print(f"\n=== Total benchmark time: {total_time:.2f} ms ===")