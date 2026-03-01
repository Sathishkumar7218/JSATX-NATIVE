import pandas as pd
import numpy as np

# Generate large dataset
np.random.seed(42)
n_rows = 100000

data = {
    'id': range(n_rows),
    'value1': np.random.randn(n_rows) * 100,
    'value2': np.random.randn(n_rows) * 50 + 100,
    'category': np.random.choice(['A', 'B', 'C', 'D'], n_rows)
}

df = pd.DataFrame(data)
df.to_csv('java/jsatx-java/large_sample.csv', index=False)
print(f"Generated large dataset with {n_rows} rows")