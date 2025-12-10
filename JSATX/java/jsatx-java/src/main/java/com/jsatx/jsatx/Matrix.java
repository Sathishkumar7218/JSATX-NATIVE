package com.jsatx.jsatx;

/**
 * Thin Java wrapper for native NDArray used as Matrix.
 * Constructor takes native pointer and dimensions (rows, cols).
 *
 * Relies on NativeLib JNI functions:
 *   ndarrayGet, ndarraySum, ndarrayMean, ndarrayDot, ndarrayFree
 */
public class Matrix {

    private final long ptr; // native NDArray*
    private final int rows;
    private final int cols;

    public Matrix(long ptr, int rows, int cols) {
        this.ptr = ptr;
        this.rows = rows;
        this.cols = cols;
    }

    /** natural print with 3 decimal places */
    public void print() {
        for (int r = 0; r < rows; r++) {
            StringBuilder sb = new StringBuilder();
            for (int c = 0; c < cols; c++) {
                sb.append(
                    String.format("%.3f  ", NativeLib.ndarrayGet(ptr, r, c))
                );
            }
            System.out.println(sb.toString().trim());
        }
    }

    public Matrix dot(Matrix other) {
        long outPtr = NativeLib.ndarrayDot(this.ptr, other.ptr);
        return new Matrix(outPtr, this.rows, other.cols);
    }

    public double sum() {
        return NativeLib.ndarraySum(ptr);
    }

    public double mean() {
        return NativeLib.ndarrayMean(ptr);
    }

    /** free native memory for this NDArray */
    public void close() {
        NativeLib.ndarrayFree(ptr);
    }

    public int rows() {
        return rows;
    }

    public int cols() {
        return cols;
    }
}
