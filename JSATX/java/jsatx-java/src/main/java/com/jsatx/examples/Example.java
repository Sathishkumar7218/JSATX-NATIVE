package com.jsatx.examples;

import com.jsatx.columns.NumericColumn;
import com.jsatx.columns.StringColumn;
import com.jsatx.jsatx.DataFrame;
import com.jsatx.jsatx.Matrix;
import com.jsatx.jsatx.NativeLib;

public class Example {

    public static void main(String[] args) {
        System.out.println("============================================");
        System.out.println("          JSATX — Native Data Engine");
        System.out.println("============================================\n");

        long startTime = System.nanoTime();

        DataFrame df = new DataFrame("sample.csv");

        System.out.println("\n--- df.head(5) ---");
        df.head(5);

        System.out.println("\n--- df.describe() ---");
        df.describe();

        System.out.println("\n--- Numeric column: Age ---");
        System.out.println("Age size: " + df.size("Age"));
        System.out.println("Age sum : " + df.sum("Age"));
        System.out.println("Age mean: " + df.mean("Age"));

        System.out.println("\nAge values:");
        double[] ages = df.valuesNumeric("Age");
        for (double age : ages) {
            System.out.println("  " + age);
        }

        System.out.println("\n--- String column: City ---");
        String[] cities = df.values("City");
        System.out.println("Cities:");
        for (String city : cities) {
            System.out.println("  " + city);
        }

        System.out.println("\n--- Matrix demo ---");
        Matrix A = new Matrix(NativeLib.ndarrayOnes(2, 3), 2, 3);
        Matrix B = new Matrix(NativeLib.ndarrayOnes(3, 2), 3, 2);

        System.out.println("A:");
        A.print();
        System.out.println("B:");
        B.print();

        Matrix C = A.dot(B);
        System.out.println("A.dot(B):");
        C.print();
        System.out.println("C sum: " + C.sum() + " mean: " + C.mean());

        // cleanup
        df.close();
        A.close();
        B.close();
        C.close();

        long endTime = System.nanoTime();
        double duration = (endTime - startTime) / 1_000_000.0; // milliseconds
        System.out.println("\n=== Demo finished in " + duration + " ms ===");
    }
}
