package com.jsatx.jsatx;

/**
 * JNI bindings to the native JSATX engine (C code).
 * Every method here directly maps to a function inside mlbase_jni.c.
 */
public final class NativeLib {

    static {
        NativeLoader.load(); // loads libjsatx.so
    }

    private NativeLib() {}

    /* ============================================
       DataFrame API
       ============================================ */

    // Load CSV → returns pointer to DataFrame*
    public static native long dfReadCsv(String path, char delimiter);

    // Print first n rows
    public static native void dfHead(long dfPtr, int n);

    // Summary statistics
    public static native void dfDescribe(long dfPtr);

    // Free DataFrame*
    public static native void dfFree(long dfPtr);

    // Get type of a column by name (0 = NUMERIC, 1 = STRING)
    public static native int dfGetColumnType(long dfPtr, String colName);

    // Numeric column → returns NDArray* pointer
    public static native long dfGetNumericColumn(long dfPtr, String colName);

    // Fast column aggregations (avoid copying large columns)
    public static native double dfSumColumn(long dfPtr, String colName);
    public static native double dfMeanColumn(long dfPtr, String colName);

    // String column → returns internal char** pointer
    public static native long dfGetStringColumn(long dfPtr, String colName);

    // Number of rows in string column
    public static native int dfGetStringColumnSize(long dfPtr, String colName);

    // Get one string value from a string column (via pointer)
    public static native String dfStringAt(long strArrayPtr, int index);

    /* ============================================
       NDArray API (Matrix)
       ============================================ */

    // Create empty NDArray
    public static native long ndarrayNew(int rows, int cols);

    // Create NDArray of zeros
    public static native long ndarrayZeros(int rows, int cols);

    // Create NDArray of ones
    public static native long ndarrayOnes(int rows, int cols);

    // Free NDArray*
    public static native void ndarrayFree(long ptr);

    // Get/set element
    public static native double ndarrayGet(long ptr, int r, int c);

    public static native void ndarraySet(long ptr, int r, int c, double v);

    // Aggregation
    public static native double ndarraySum(long ptr);

    public static native double ndarrayMean(long ptr);

    // Matrix dot product
    public static native long ndarrayDot(long aPtr, long bPtr);
}
