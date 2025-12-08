package com.jsatx.jsatx;

/**
 * JSATX Matrix wrapper over native NDArray.
 * NumPy-like operations exposed to Java.
 */
public class Matrix {

    private final long ptr;

    public Matrix(long nativePtr) {
        this.ptr = nativePtr;
    }

    public static Matrix zeros(int rows, int cols) {
        long p = NativeLib.ndarrayZeros(rows, cols);
        return new Matrix(p);
    }

    public static Matrix ones(int rows, int cols) {
        long p = NativeLib.ndarrayOnes(rows, cols);
        return new Matrix(p);
    }

    public static Matrix array(double[][] values) {
        int r = values.length;
        int c = values[0].length;
        long p = NativeLib.ndarrayNew(r, c);
        Matrix m = new Matrix(p);

        for (int i = 0; i < r; i++) for (
            int j = 0;
            j < c;
            j++
        ) NativeLib.ndarraySet(p, i, j, values[i][j]);

        return m;
    }

    public double get(int r, int c) {
        return NativeLib.ndarrayGet(ptr, r, c);
    }

    public void set(int r, int c, double v) {
        NativeLib.ndarraySet(ptr, r, c, v);
    }

    public Matrix dot(Matrix other) {
        long p = NativeLib.ndarrayDot(this.ptr, other.ptr);
        return new Matrix(p);
    }

    public double sum() {
        return NativeLib.ndarraySum(ptr);
    }

    public double mean() {
        return NativeLib.ndarrayMean(ptr);
    }

    public long nativePtr() {
        return ptr;
    }

    public void close() {
        NativeLib.ndarrayFree(ptr);
    }
}
