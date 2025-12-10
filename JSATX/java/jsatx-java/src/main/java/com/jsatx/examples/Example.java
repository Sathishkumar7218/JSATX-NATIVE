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

        DataFrame df = new DataFrame("sample.csv");

        System.out.println("\n--- df.head(5) ---");
        df.head(5);

        System.out.println("\n--- df.describe() ---");
        df.describe();

        System.out.println("\n--- Numeric column: Age ---");
        NumericColumn age = df.getNumeric("Age");
        System.out.println("Age size: " + age.size());
        System.out.println("Age sum : " + age.sum());
        System.out.println("Age mean: " + age.mean());

        System.out.println("\nAge values:");
        for (int i = 0; i < age.size(); i++) {
            System.out.println("  " + age.get(i));
        }

        System.out.println("\n--- String column: City ---");
        StringColumn city = df.getString("City");
        System.out.println("Cities:");
        for (int i = 0; i < city.size(); i++) {
            System.out.println("  " + city.get(i));
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

        System.out.println("\n=== Demo finished ===");
    }
}
