package com.jsatx.jsatx;

import com.jsatx.columns.*;
import java.util.*;

public class DataFrame {

    private long nativePtr; // pointer to C DataFrame

    public DataFrame(String path) {
        this.nativePtr = NativeLib.dfReadCsv(path, ',');
        if (nativePtr == 0) throw new RuntimeException(
            "Failed to load CSV: " + path
        );
    }

    public void head(int n) {
        NativeLib.dfHead(nativePtr, n);
    }

    public void describe() {
        NativeLib.dfDescribe(nativePtr);
    }

    /**
     * MAIN FIX — Auto-detect type based on native column type
     */
    public Column get(String name) {
        int type = NativeLib.dfGetColumnType(nativePtr, name);

        if (type == 0) {
            // NUMERIC
            long ndarrayPtr = NativeLib.dfGetNumericColumn(nativePtr, name);
            return new NumericColumn(name, ndarrayPtr);
        } else if (type == 1) {
            // STRING
            String[] data = NativeLib.dfGetStringColumn(nativePtr, name);
            return new StringColumn(name, data);
        } else {
            throw new RuntimeException("Column not found: " + name);
        }
    }

    public void free() {
        NativeLib.dfFree(nativePtr);
    }
}
