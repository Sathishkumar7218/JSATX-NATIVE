package com.jsatx.jsatx;

import java.io.*;
import java.nio.file.Files;

/**
 * Loads native library "libjsatx.so".
 */
public final class NativeLoader {

    private NativeLoader() {}

    public static void load() {
        // First prefer the normal system library load (e.g. already installed on PATH)
        try {
            System.loadLibrary("jsatx");
            return;
        } catch (UnsatisfiedLinkError ignored) {}

        // Determine platform (os + arch)
        String os = System.getProperty("os.name").toLowerCase();
        String arch = System.getProperty("os.arch").toLowerCase();
        String platform;
        String libName;

        if (os.contains("linux")) {
            platform = arch.contains("aarch64") || arch.contains("arm64") ? "linux-arm64" : "linux-x86_64";
            libName = "libjsatx.so";
        } else if (os.contains("mac") || os.contains("darwin")) {
            platform = arch.contains("aarch64") || arch.contains("arm64") ? "macos-arm64" : "macos-x86_64";
            libName = "libjsatx.dylib";
        } else if (os.contains("win")) {
            platform = arch.contains("64") ? "windows-x86_64" : "windows-x86";
            libName = "jsatx.dll";
        } else {
            platform = "unknown";
            libName = "libjsatx.so"; // fallback
        }

        // Try platform-specific resource first
        String[] candidates = new String[] {
            "/natives/" + platform + "/" + libName,
            "/natives/" + libName
        };

        for (String res : candidates) {
            InputStream in = NativeLoader.class.getResourceAsStream(res);
            if (in == null) continue;
            try {
                File tmp = Files.createTempFile("libjsatx-", "-" + libName).toFile();
                tmp.deleteOnExit();
                try (OutputStream out = new FileOutputStream(tmp)) {
                    byte[] buf = new byte[8192];
                    int r;
                    while ((r = in.read(buf)) != -1) out.write(buf, 0, r);
                }
                System.load(tmp.getAbsolutePath());
                return;
            } catch (IOException e) {
                throw new UnsatisfiedLinkError("Failed to extract: " + e.getMessage());
            }
        }

        throw new UnsatisfiedLinkError("Native library not found for platform: " + platform);
    }
}
