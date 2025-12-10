package com.jsatx.columns;

import com.jsatx.jsatx.Matrix;
import com.jsatx.jsatx.NativeLib;

/**
 * Numeric column backed by a native NDArray pointer.
 * Simple API: get(i), sum(), mean(), size(), toMatrix()
 */
public class NumericColumn {

    private final String name;
    private final long ndarrayPtr; // native NDArray*
    private final int size;

    public NumericColumn(String name, long ndarrayPtr, int size) {
        this.name = name;
        this.ndarrayPtr = ndarrayPtr;
        this.size = size;
    }

    public String getName() {
        return name;
    }

    /** Number of rows */
    public int size() {
        return size;
    }

    /** Get value at index i (0-based) */
    public double get(int i) {
        // NDArray used as column vector: column index is 0
        return NativeLib.ndarrayGet(ndarrayPtr, i, 0);
    }

    public double sum() {
        return NativeLib.ndarraySum(ndarrayPtr);
    }

    public double mean() {
        return NativeLib.ndarrayMean(ndarrayPtr);
    }

    /** Wrap as Matrix (rows x 1) */
    public Matrix toMatrix() {
        return new Matrix(ndarrayPtr, size, 1);
    }
}
