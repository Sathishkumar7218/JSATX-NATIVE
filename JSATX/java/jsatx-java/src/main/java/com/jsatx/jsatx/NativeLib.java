package com.jsatx.jsatx;

/**
 * Native method declarations matching mlbase_jni.c
 */
public final class NativeLib {

    static {
        NativeLoader.load();
    }

    private NativeLib() {}

    /* DataFrame */
    public static native long dfReadCsv(String path, char delimiter);
    public static native void dfHead(long dfPtr, int n);
    public static native void dfDescribe(long dfPtr);
    public static native int dfGetColumnType(long dfPtr, String name);
    public static native long dfGetNumericColumn(long dfPtr, String name);
    public static native String[] dfGetStringColumn(long dfPtr, String name);
    public static native void dfFree(long dfPtr);

    /* NDArray */
    public static native long ndarrayNew(int rows, int cols);
    public static native long ndarrayZeros(int rows, int cols);
    public static native long ndarrayOnes(int rows, int cols);
    public static native void ndarrayFree(long arrPtr);
    public static native double ndarrayGet(long arrPtr, int r, int c);
    public static native void ndarraySet(long arrPtr, int r, int c, double value);
    public static native double ndarraySum(long arrPtr);
    public static native double ndarrayMean(long arrPtr);
    public static native long ndarrayDot(long aPtr, long bPtr);
}
