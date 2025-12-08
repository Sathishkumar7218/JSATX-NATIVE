package com.jsatx.columns;

public abstract class Column {

    protected String name;

    public Column(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public boolean isNumeric() {
        return false;
    }

    public boolean isString() {
        return false;
    }

    public long asNumeric() {
        throw new RuntimeException("Not numeric");
    }

    public String[] asString() {
        throw new RuntimeException("Not string");
    }
}
