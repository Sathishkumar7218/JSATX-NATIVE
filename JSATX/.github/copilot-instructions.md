# Copilot / AI Agent Instructions for JSATX

This file provides focused, actionable guidance for AI coding agents working in this repository.

- **Big picture**: The project is a small Java library (`com.jsatx`) with a native C backend (JNI) that implements DataFrame and NDArray primitives. Java lives under `java/jsatx-java` and native code is under `native/`.

- **Key files**:
  - Java build: [java/jsatx-java/pom.xml](java/jsatx-java/pom.xml)
  - JNI bindings: [java/jsatx-java/src/main/java/com/jsatx/jsatx/NativeLib.java](java/jsatx-java/src/main/java/com/jsatx/jsatx/NativeLib.java)
  - Native JNI glue: [native/src/mlbase_jni.c](native/src/mlbase_jni.c)
  - Core native impls: [native/src/dataframe.c](native/src/dataframe.c), [native/src/ndarray.c](native/src/ndarray.c)
  - Native build: [native/Makefile](native/Makefile)
  - Native header: [native/include/mlbase.h](native/include/mlbase.h)

- **Build / test / dev workflow** (exact commands):
  - Build Java: `mvn -f java/jsatx-java clean package`
  - Run Java tests: `mvn -f java/jsatx-java test` (tests under `java/jsatx-java/src/test`)
  - Build native library: `make -C native` (runs `gcc` with JNI include flags)
  - Install native .so into Java resources for packaging: `make -C native install` (copies `libjsatx.so` to `java/jsatx-java/src/main/resources/natives/`)

- **How JNI integration works (important for edits)**:
  - `NativeLib.java` declares `native` methods. These map to C functions named `Java_com_jsatx_jsatx_NativeLib_*` implemented in `native/src/mlbase_jni.c`.
  - Native implementations call into the core C modules (`dataframe.c`, `ndarray.c`). Keep Java package and method names in sync with JNI function names; changing package/method names requires updating JNI symbols.
  - Java objects store raw native pointers as `long` (e.g. `DataFrame.ptr`). These point to heap-allocated C structs; memory must be freed via the corresponding native `free` functions (`dfFree`, `ndarrayFree`).

- **Project-specific conventions & patterns**:
  - Numeric vs string column typing: `dfGetColumnType` returns `0` = numeric, `1` = string. Java `DataFrame.getNumeric` checks this before using numeric APIs.
  - Native allocation pattern: JNI functions often `malloc` an `NDArray`/`DataFrame` and return its pointer as `jlong`; the Java side is responsible for calling the matching `Free` native method.
  - Native build uses `JAVA_HOME` defaulting to `/usr/lib/jvm/java-21-openjdk` in `native/Makefile`. Agents should respect or detect `JAVA_HOME` when compiling.

- **Notable mismatch to watch for** (discovered in repo):
  - `native/Makefile` installs `libjsatx.so` into `java/jsatx-java/src/main/resources/natives/`.
  - `NativeLoader` first tries `System.loadLibrary("jsatxnative")` and, on fallback, attempts to extract a resource at `/native/libjsatxnative.so`.
  - These names/paths differ (library basename `libjsatx.so` vs `libjsatxnative.so` and resource dirs `natives/` vs `native/`). Agents should avoid blindly renaming libraries or changing `NativeLoader` without verifying packaging and resource paths.

- **Editing guidance (do this, not that)**:
  - Do: When adding new native functions, add the Java `native` declaration, then add the corresponding JNI function in `mlbase_jni.c` using the exact mangled name. Update `native/include/mlbase.h` if new types are exported.
  - Do: Run `make -C native && make -C native install && mvn -f java/jsatx-java test` after making JNI/native changes to validate integration.
  - Don't: Change Java package names or method signatures without updating JNI symbols and rebuilding the native library.

- **Quick pointers for common tasks**:
  - Where to look for examples: [`java/jsatx-java/src/main/java/com/jsatx/jsatx/DataFrame.java`](java/jsatx-java/src/main/java/com/jsatx/jsatx/DataFrame.java) and [`java/jsatx-java/src/main/java/com/jsatx/jsatx/NativeLib.java`](java/jsatx-java/src/main/java/com/jsatx/jsatx/NativeLib.java) show usage patterns and pointer lifetimes.
  - Where native tests/examples live: there are no formal native unit tests; verify via Java integration tests or tiny C programs under `native/src/`.

If anything above is unclear or you want me to include automated test/run snippets (examples of running `Example.java`), tell me which area to expand and I'll iterate.
