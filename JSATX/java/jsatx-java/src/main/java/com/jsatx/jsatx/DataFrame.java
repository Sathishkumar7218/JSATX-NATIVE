package com.jsatx.jsatx;

import com.jsatx.columns.NumericColumn;
import com.jsatx.columns.StringColumn;

/**
 * Minimal, pandas-like DataFrame API (Option A).
 *
 * Usage:
 *   DataFrame df = new DataFrame("sample.csv");
 *   df.head(5);
 *   NumericColumn age = df.getNumeric("Age");
 *   StringColumn city = df.getString("City");
 */
public class DataFrame {

    private final long ptr; // native DataFrame*

    public DataFrame(String path) {
        this.ptr = NativeLib.dfReadCsv(path, ',');
        if (this.ptr == 0) {
            throw new RuntimeException("Failed to load CSV: " + path);
        }
    }

    /** Print first n rows (delegated to native printer) */
    public void head(int n) {
        NativeLib.dfHead(ptr, n);
    }

    /** Describe numeric columns (delegated to native describe) */
    public void describe() {
        NativeLib.dfDescribe(ptr);
    }

    /**
     * Return a NumericColumn for the named column.
     * Throws RuntimeException if the column is not numeric or not found.
     */
    public NumericColumn getNumeric(String name) {
        int type = NativeLib.dfGetColumnType(ptr, name);
        if (type != 0) {
            // 0 = numeric (as per NativeLib)
            throw new RuntimeException("Column is not numeric: " + name);
        }
        long ndPtr = NativeLib.dfGetNumericColumn(ptr, name);
        int rows = NativeLib.dfGetStringColumnSize(ptr, name); // returns row count
        return new NumericColumn(name, ndPtr, rows);
    }

    /**
     * Return a StringColumn for the named column.
     * Throws RuntimeException if the column is not string or not found.
     */
    public StringColumn getString(String name) {
        int type = NativeLib.dfGetColumnType(ptr, name);
        if (type != 1) {
            // 1 = string (as per NativeLib)
            throw new RuntimeException("Column is not string: " + name);
        }
        long strArrayPtr = NativeLib.dfGetStringColumn(ptr, name);
        int rows = NativeLib.dfGetStringColumnSize(ptr, name);
        return new StringColumn(name, strArrayPtr, rows);
    }

    // Convenience methods for Python-like syntax

    /** Get sum of numeric column (fast, avoids copying) */
    public double sum(String colName) {
        return NativeLib.dfSumColumn(ptr, colName);
    }

    /** Get mean of numeric column (fast, avoids copying) */
    public double mean(String colName) {
        return NativeLib.dfMeanColumn(ptr, colName);
    }

    /** Get size of any column */
    public int size(String colName) {
        int type = NativeLib.dfGetColumnType(ptr, colName);
        if (type == 0) {
            return getNumeric(colName).size();
        } else if (type == 1) {
            return getString(colName).size();
        } else {
            throw new RuntimeException("Unknown column type for: " + colName);
        }
    }

    /** Get all values of string column as array */
    public String[] values(String colName) {
        StringColumn col = getString(colName);
        String[] vals = new String[col.size()];
        for (int i = 0; i < col.size(); i++) {
            vals[i] = col.get(i);
        }
        return vals;
    }

    /** Get all values of numeric column as array */
    public double[] valuesNumeric(String colName) {
        NumericColumn col = getNumeric(colName);
        double[] vals = new double[col.size()];
        for (int i = 0; i < col.size(); i++) {
            vals[i] = col.get(i);
        }
        return vals;
    }

    /** Free native resources */
    public void close() {
        NativeLib.dfFree(ptr);
    }
}
