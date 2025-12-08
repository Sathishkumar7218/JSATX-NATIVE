package com.jsatx.examples;

import com.jsatx.columns.*;
import com.jsatx.jsatx.DataFrame;

public class Example {

    public static void main(String[] args) {
        System.out.println("=== JSATX DataFrame Demo ===");

        DataFrame df = new DataFrame("sample.csv");

        System.out.println("\n--- head(5) ---");
        df.head(5);

        System.out.println("\n--- describe() ---");
        df.describe();

        System.out.println("\n--- Accessing Numeric Column \"Age\" ---");
        NumericColumn age = (NumericColumn) df.get("Age");

        System.out.println("Age mean: " + age.mean());
        System.out.println("Age sum : " + age.sum());

        System.out.println("\n--- Accessing String Column \"City\" ---");
        StringColumn cities = (StringColumn) df.get("City");

        for (int i = 0; i < cities.size(); i++) {
            System.out.println(cities.get(i));
        }

        df.free();
    }
}
