import pandas as pd
import numpy as np
import time

print("============================================")
print("          Pandas/Numpy Benchmark")
print("          (Python Equivalent)")
print("============================================\n")

start_time = time.time()

# Load CSV
df = pd.read_csv("java/jsatx-java/sample.csv")

print("--- df.head(5) ---")
print(df.head(5))

print("\n--- df.describe() ---")
print(df.describe())

print("\n--- Numeric column: Age ---")
age = df['Age']
print(f"Age size: {len(age)}")
print(f"Age sum : {age.sum()}")
print(f"Age mean: {age.mean()}")

print("\nAge values:")
for val in age:
    print(f"  {val}")

print("\n--- String column: City ---")
city = df['City']
print("Cities:")
for c in city:
    print(f"  {c}")

print("\n--- Matrix demo ---")
A = np.ones((2, 3))
B = np.ones((3, 2))

print("A:")
print(A)
print("B:")
print(B)

C = np.dot(A, B)
print("A.dot(B):")
print(C)
print(f"C sum: {C.sum()} mean: {C.mean()}")

end_time = time.time()
duration = (end_time - start_time) * 1000  # milliseconds
print(f"\n=== Demo finished in {duration:.2f} ms ===")