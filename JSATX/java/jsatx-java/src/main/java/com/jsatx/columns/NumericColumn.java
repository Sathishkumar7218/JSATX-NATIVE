package com.jsatx.columns;

import com.jsatx.jsatx.NativeLib;

public class NumericColumn extends Column {

    private final long ptr; // NDArray pointer

    public NumericColumn(String name, long ptr) {
        super(name);
        this.ptr = ptr;
    }

    @Override
    public boolean isNumeric() {
        return true;
    }

    @Override
    public boolean isString() {
        return false;
    }

    @Override
    public long asNumeric() {
        return ptr;
    }

    /** Compute mean using native C function */
    public double mean() {
        return NativeLib.ndarrayMean(ptr);
    }

    /** Compute sum using native C function */
    public double sum() {
        return NativeLib.ndarraySum(ptr);
    }
}
