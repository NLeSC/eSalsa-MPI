package cesm2;

public class Logging {

    private static long start;
    
    static void start() { 
        start = System.currentTimeMillis();
        System.out.println("0 : Logging started");
    }
    
    static void stop() { 
        System.out.println((System.currentTimeMillis() - start) + " : Logging stopped");
    }
    
    static void println(String s) { 
        System.out.println((System.currentTimeMillis() - start) + " : " + s);
    }

    static void error(String s) { 
        System.out.println((System.currentTimeMillis() - start) + " [ERROR] : " + s);        
    }
}
