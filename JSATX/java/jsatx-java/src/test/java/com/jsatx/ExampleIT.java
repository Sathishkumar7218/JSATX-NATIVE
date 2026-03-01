package com.jsatx;

import junit.framework.TestCase;
import com.jsatx.examples.Example;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

/**
 * Integration test that runs the Example main to ensure JNI loads and executes.
 */
public class ExampleIT extends TestCase {

    public void testExampleRuns() {
        PrintStream oldOut = System.out;
        ByteArrayOutputStream baos = new ByteArrayOutputStream();
        System.setOut(new PrintStream(baos));
        try {
            Example.main(new String[0]);
            String out = baos.toString();
            assertTrue("Example output should contain demo header", out.contains("JSATX — Native Data Engine"));
            assertTrue("Example should finish", out.contains("Demo finished"));
        } finally {
            System.setOut(oldOut);
        }
    }
}
