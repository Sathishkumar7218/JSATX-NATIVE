package com.jsatx.columns;

import com.jsatx.jsatx.NativeLib;

/**
 * String column backed by native char** pointer.
 * Simple API: get(i), size()
 */
public class StringColumn {

    private final String name;
    private final long strArrayPtr; // pointer returned by dfGetStringColumn
    private final int size;

    public StringColumn(String name, long strArrayPtr, int size) {
        this.name = name;
        this.strArrayPtr = strArrayPtr;
        this.size = size;
    }

    public String getName() {
        return name;
    }

    public int size() {
        return size;
    }

    /** Returns the string at index (0-based) using JNI helper */
    public String get(int i) {
        return NativeLib.dfStringAt(strArrayPtr, i);
    }
}
