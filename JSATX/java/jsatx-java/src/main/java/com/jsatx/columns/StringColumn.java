package com.jsatx.columns;

public class StringColumn extends Column {

    private final String[] data;

    public StringColumn(String name, String[] values) {
        super(name);
        this.data = values;
    }

    @Override
    public boolean isString() {
        return true;
    }

    @Override
    public boolean isNumeric() {
        return false;
    }

    @Override
    public String[] asString() {
        return data;
    }

    public String get(int index) {
        return data[index];
    }

    public int size() {
        return data.length;
    }
}
