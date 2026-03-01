package com.jsatx;

import junit.framework.TestCase;

import java.io.File;
import java.io.IOException;
import java.util.Enumeration;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

/**
 * Integration test that verifies the packaged JAR contains platform native directories.
 * This test is executed by the Failsafe plugin during the verify phase (after package).
 */
public class JarContentsIT extends TestCase {

    public void testFatJarContainsPlatformNatives() throws IOException {
        File jar = new File("target/jsatx-java-1.0-SNAPSHOT.jar");
        assertTrue("Packaged JAR not found at: " + jar.getAbsolutePath(), jar.exists());

        JarFile jf = new JarFile(jar);
        boolean found = false;

        // Determine current platform
        String os = System.getProperty("os.name").toLowerCase();
        String arch = System.getProperty("os.arch").toLowerCase();
        String platform;
        if (os.contains("linux")) {
            platform = arch.contains("aarch64") || arch.contains("arm64") ? "linux-arm64" : "linux-x86_64";
        } else if (os.contains("mac") || os.contains("darwin")) {
            platform = arch.contains("aarch64") || arch.contains("arm64") ? "macos-arm64" : "macos-x86_64";
        } else if (os.contains("win")) {
            platform = arch.contains("64") ? "windows-x86_64" : "windows-x86";
        } else {
            platform = "linux-x86_64"; // conservative fallback
        }

        Enumeration<JarEntry> en = jf.entries();
        while (en.hasMoreElements()) {
            JarEntry e = en.nextElement();
            String name = e.getName();
            if (name.startsWith("natives/" + platform + "/")) {
                found = true;
                break;
            }
        }
        jf.close();

        assertTrue("Fat JAR must include natives for current platform: " + platform, found);
    }
}
