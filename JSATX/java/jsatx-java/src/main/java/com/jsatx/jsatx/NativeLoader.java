package com.jsatx.jsatx;

import java.io.*;
import java.nio.file.Files;

/**
 * Loads native library "libjsatx.so".
 */
public final class NativeLoader {

    private NativeLoader() {}

    public static void load() {
        try {
            System.loadLibrary("jsatx");
            return;
        } catch (UnsatisfiedLinkError ignored) {}

        String res = "/natives/libjsatx.so";
        InputStream in = NativeLoader.class.getResourceAsStream(res);
        if (in == null) {
            throw new UnsatisfiedLinkError("Native library not found: " + res);
        }
        try {
            File tmp = Files.createTempFile("libjsatx-", ".so").toFile();
            tmp.deleteOnExit();
            try (OutputStream out = new FileOutputStream(tmp)) {
                byte[] buf = new byte[8192];
                int r;
                while ((r = in.read(buf)) != -1) out.write(buf, 0, r);
            }
            System.load(tmp.getAbsolutePath());
        } catch (IOException e) {
            throw new UnsatisfiedLinkError(
                "Failed to extract: " + e.getMessage()
            );
        }
    }
}
