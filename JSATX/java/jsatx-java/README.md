# jsatx-java — Usage

Add this module to your Maven project (after running `mvn install` locally):

```xml
<dependency>
  <groupId>com.jsatx</groupId>
  <artifactId>jsatx-java</artifactId>
  <version>1.0-SNAPSHOT</version>
</dependency>
```

Notes:
- The native library is packaged inside the JAR under platform-specific paths, for example `/natives/linux-x86_64/libjsatx.so`, `/natives/macos-x86_64/libjsatx.dylib`, and `/natives/windows-x86_64/jsatx.dll`. `NativeLoader` will detect the runtime OS/arch and extract the matching binary automatically.
- To rebuild and install a native binary for your platform locally: `make -C native PLATFORM=linux-x86_64 install`, `PLATFORM=macos-x86_64`, or `PLATFORM=windows-x86_64` (Windows builds require MSYS2/mingw toolchain).
- CI will build and publish per-platform artifacts to GitHub Packages using classifiers (e.g. `com.jsatx:jsatx-java:1.0-SNAPSHOT:linux-x86_64`). Consumers can depend on the classifier matching their platform, or use platform-aware resolution when consuming artifacts.
- Additionally, CI will assemble and publish a **fat JAR** that contains the native binaries for all supported platforms inside `/natives/`. This means consumers can simply add the usual dependency `com.jsatx:jsatx-java:1.0-SNAPSHOT` and `NativeLoader` will select the correct native at runtime — no extra configuration required.
- To run the example locally: `./scripts/run-example.sh` (builds/installs native lib then uses `mvn exec:java`).

## ✅ Optional: Enable OpenBLAS (faster matrix multiply)

If you want the fastest matrix multiply path, install your platform's OpenBLAS development package so the native build will detect and link cblas (dgemm). After installing, rebuild the native library and run the benchmark to verify speedups.

- Install OpenBLAS dev package (examples):
  - Fedora/RHEL: `sudo dnf install openblas-devel`
  - Debian/Ubuntu: `sudo apt update && sudo apt install -y libopenblas-dev`
  - Arch Linux: `sudo pacman -Syu openblas`
  - macOS (Homebrew): `brew install openblas`
  - Windows (MSYS2/MinGW): `pacman -S mingw-w64-x86_64-openblas`

- Rebuild and install native + build Java jar:

```bash
# rebuild native for your platform (example for Linux x86_64)
make -C native clean
make -C native PLATFORM=linux-x86_64
make -C native install PLATFORM=linux-x86_64

# build Java package (skip tests for speed)
mvn -DskipTests package
```

- Run the benchmark (reports matrix and total times)

Use the commands below from the repository root (recommended) so relative paths work reliably.

```bash
# Build native and Java (example for Linux x86_64)
make -C native clean
make -C native PLATFORM=linux-x86_64
make -C native install PLATFORM=linux-x86_64
mvn -f java/jsatx-java -DskipTests package

# Run the JSATX benchmark (from repo root)
cd /home/robin/'vs code'/Final_year/JSATX/java/jsatx-java
java -cp target/classes com.jsatx.examples.Benchmark
# or use the helper script (also from repo root)
./scripts/run-benchmarks.sh
```

If you prefer to run from the `java/jsatx-java` directory, use these commands instead:

```bash
# from java/jsatx-java
mvn -DskipTests package
java -cp target/classes com.jsatx.examples.Benchmark
# to run the Python benchmark from here (note the relative path)
python3 ../python_benchmark.py
```

- Run the Python benchmark (Pandas/NumPy) directly from the repository root:

```bash
# (optional) create and activate a virtualenv
python3 -m venv .venv && source .venv/bin/activate
# install dependencies
pip install -U pip
pip install pandas numpy
# run the Python benchmark from the repositorthey root
cd /home/robin/'vs code'/Final_year/JSATX
python3 python_benchmark.py
```

- Notes:j
  - The Python benchmark reads `java/jsatx-java/large_sample.csv` so ensure you're running it from the repository root or pass the correct path when running from other directories.
  - Avoid absolute paths that include spaces in shell commands (or quote them) — prefer relative paths shown above.

- Notes:
  - The Python benchmark reads `java/jsatx-java/large_sample.csv` so ensure you're in the repository root when running it.
  - NumPy may use an optimized BLAS (OpenBLAS/MKL) depending on how it was installed; results can vary accordingly.

- How to verify BLAS was used:
  - During `make` you should see the message: `[INFO] cblas.h found; enabling BLAS acceleration`.
  - In the benchmark output, the **Matrix ops time** should be noticeably lower when BLAS is active (cblas_dgemm is used under the hood).

Publishing to GitHub Packages (automatic via GitHub Actions)

- A workflow has been added at `.github/workflows/publish.yml` which will:
  - build and install the native library into `src/main/resources/natives/`
  - package the JAR
  - deploy the artifact to GitHub Packages using the repository's `GITHUB_TOKEN` when you push a tag like `v1.0.0` or trigger the workflow manually.

Notes & repo setup:
- The workflow uses `mvn deploy` with an alternate repository URL `https://maven.pkg.github.com/<owner>/<repo>`.
- Ensure `Actions` can write packages: go to your repository Settings → Actions → General and verify Permissions include `Read and write permissions` for `GITHUB_TOKEN` (the workflow already requests `packages: write`).
- After the workflow runs, consumers can add the GitHub Packages repository to their `pom.xml` and depend on `com.jsatx:jsatx-java:1.0-SNAPSHOT`.

If you want me to add multi-platform packaging (per-OS/native artifacts), I can add profiles to automate that. Let me know.