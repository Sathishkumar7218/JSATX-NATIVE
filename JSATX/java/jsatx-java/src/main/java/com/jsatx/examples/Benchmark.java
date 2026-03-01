package com.jsatx.examples;

import com.jsatx.jsatx.DataFrame;
import com.jsatx.jsatx.Matrix;
import com.jsatx.jsatx.NativeLib;

public class Benchmark {

    public static void main(String[] args) {
        System.out.println("============================================");
        System.out.println("          JSATX — Large Data Benchmark");
        System.out.println("============================================\n");

        // Load DataFrame
        System.out.println("Loading large CSV...");
        long loadStart = System.nanoTime();
        DataFrame df = new DataFrame("large_sample.csv");
        long loadEnd = System.nanoTime();
        double loadTime = (loadEnd - loadStart) / 1_000_000.0;
        System.out.println("Load time: " + loadTime + " ms");

        // DataFrame operations
        System.out.println("\n--- DataFrame Operations ---");
        long dfStart = System.nanoTime();

        double sum1 = df.sum("value1");
        double mean1 = df.mean("value1");
        double sum2 = df.sum("value2");
        double mean2 = df.mean("value2");
        int size = df.size("id");

        long dfEnd = System.nanoTime();
        double dfTime = (dfEnd - dfStart) / 1_000_000.0;

        System.out.println("Rows: " + size);
        System.out.println("value1 sum: " + sum1);
        System.out.println("value1 mean: " + mean1);
        System.out.println("value2 sum: " + sum2);
        System.out.println("value2 mean: " + mean2);
        System.out.println("DataFrame ops time: " + dfTime + " ms");

        // Matrix operations
        System.out.println("\n--- Matrix Operations ---");
        int matrixSize = 500;
        System.out.println("Creating " + matrixSize + "x" + matrixSize + " matrices...");

        long matrixStart = System.nanoTime();

        Matrix A = new Matrix(NativeLib.ndarrayNew(matrixSize, matrixSize), matrixSize, matrixSize);
        Matrix B = new Matrix(NativeLib.ndarrayNew(matrixSize, matrixSize), matrixSize, matrixSize);

        // Fill with some values
        for (int i = 0; i < matrixSize; i++) {
            for (int j = 0; j < matrixSize; j++) {
                NativeLib.ndarraySet(A.getPtr(), i, j, Math.sin(i + j));
                NativeLib.ndarraySet(B.getPtr(), i, j, Math.cos(i - j));
            }
        }

        Matrix C = A.dot(B);
        double matrixSum = C.sum();
        double matrixMean = C.mean();

        long matrixEnd = System.nanoTime();
        double matrixTime = (matrixEnd - matrixStart) / 1_000_000.0;

        System.out.println("Matrix multiplication result sum: " + matrixSum);
        System.out.println("Matrix multiplication result mean: " + matrixMean);
        System.out.println("Matrix ops time: " + matrixTime + " ms");

        // Cleanup
        df.close();
        A.close();
        B.close();
        C.close();

        double totalTime = loadTime + dfTime + matrixTime;
        System.out.println("\n=== Total benchmark time: " + totalTime + " ms ===");
    }
}