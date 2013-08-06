package generator;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.StringTokenizer;

public class Main {

    private static String [] skip = new String [] {
        "MPI_Init", 
        "MPI_Pcontrol", 
        "MPI_Status_c2f", 
        "MPI_Status_f2c",
        "MPI_Wtime", 
        "MPI_Wtick"
    };
   
    private static String [] headerskip = new String [] {
        "MPI_Pcontrol", 
        "MPI_Status_c2f", 
        "MPI_Status_f2c", 
        "MPI_Wtime", 
        "MPI_Wtick"
    };
    
    private static String [][] sensitive = new String [][] {
        { "request", "r" },  
        { "group", "g" } 
    };
    
    private static StatsField [] fields = new StatsField [] {
        new StatsField("MPI_Allgather", "STATS_ALLGATHER"), 
        new StatsField("MPI_Allgatherv", "STATS_ALLGATHER"),
        new StatsField("MPI_Allreduce", "STATS_ALLREDUCE"),
        new StatsField("MPI_Alltoall", "STATS_ALLTOALL"),
        new StatsField("MPI_Alltoallv", "STATS_ALLTOALL"),
        new StatsField("MPI_Alltoallw", "STATS_ALLTOALL"),
        new StatsField("MPI_Barrier", "STATS_BARRIER"),
        new StatsField("MPI_Bcast", "STATS_BCAST"),
        new StatsField("MPI_Bsend", "STATS_BSEND"),
        new StatsField("MPI_Exscan", "STATS_EXSCAN"),
        new StatsField("MPI_Gather", "STATS_GATHER"),
        new StatsField("MPI_Gatherv", "STATS_GATHER"),
        new StatsField("MPI_Ibsend", "STATS_IBSEND"),
        new StatsField("MPI_Iprobe", "STATS_IPROBE"),
        new StatsField("MPI_Irecv", "STATS_IRECV"),
        new StatsField("MPI_Irsend", "STATS_IRSEND"),
        new StatsField("MPI_Isend", "STATS_ISEND"),
        new StatsField("MPI_Issend", "STATS_ISSEND"),
        new StatsField("MPI_Probe", "STATS_PROBE"),
        new StatsField("MPI_Recv", "STATS_RECV"),
        new StatsField("MPI_Reduce", "STATS_REDUCE"),
        new StatsField("MPI_Reduce_scatter", "STATS_REDUCE_SCATTER"),
        new StatsField("MPI_Rsend", "STATS_RSEND"),
        new StatsField("MPI_Scan", "STATS_SCAN"),
        new StatsField("MPI_Scatter", "STATS_SCATTER"),
        new StatsField("MPI_Scatterv", "STATS_SCATTER"),
        new StatsField("MPI_Send", "STATS_SEND"),
        new StatsField("MPI_Sendrecv", "STATS_SEND_RECV"),
        new StatsField("MPI_Sendrecv_replace", "STATS_SEND_RECV"),
        new StatsField("MPI_Ssend", "STATS_SSEND"),
        new StatsField("MPI_Wait", "STATS_WAIT"),
        new StatsField("MPI_Waitall", "STATS_WAITALL"),
        new StatsField("MPI_Waitany", "STATS_WAITANY"),
        new StatsField("MPI_Waitsome", "STATS_WAITSOME"),
        new StatsField("MPI_Test", "STATS_TEST"),
        new StatsField("MPI_Testall", "STATS_TESTALL"),
        new StatsField("MPI_Testany", "STATS_TESTANY"),
        new StatsField("MPI_Testsome", "STATS_TESTSOME"),
    };
        
    private static class StatsField {
        
        final String function;
        final String field;
        
        StatsField(String function, String field) {
            this.function = function;
            this.field = field;
        }
    }
    
    private static class TypePrinter {

        final String format;
        final String function;

        TypePrinter(String format) { 
            this(format, null);
        }

        TypePrinter(String format, String function) { 
            this.format = format;
            this.function = function;					
        }

        String getFormat() { 
            return format;
        }

        String getValue(String name) {

            if (function == null) { 
                return name;
            }

            return function + "(" + name + ")";
        }		
    }

    private static class ErrorPrinter extends TypePrinter {

        ErrorPrinter() { 
            super(null, null);
        }

        String getFormat() { 
            fatal("Cannot print this type!", null);
            return null;
        }

        String getValue(String name) {
            fatal("Cannot print this type!", null);
            return null;
        }
    } 

    private static class UnknownTypePrinter extends TypePrinter {

        UnknownTypePrinter() { 
            super("%p", null);
        }

        String getValue(String name) {			
            return "(void *) " + name;
        }
    }

    private static class Type { 
        final String name;
        final TypePrinter printer;

        Type(String name, TypePrinter printer) { 
            this.name = name;
            this.printer = printer;
        }

        String getFormat() {
            return printer.getFormat();
        }

        String getValue(String name) {
            return printer.getValue(name);
        }
    }

    private static class Parameter { 

        final boolean pointer;
        final boolean array;

        final Type type;
        final String name;

        Parameter(Type type, String name, boolean pointer, boolean array) {
            this.type = type;
            this.name = name;
            this.pointer = pointer;
            this.array = array;
        }				
    }

    private static class Function { 

        final Type ret;
        final String name;

        final Parameter [] parameters;

        final int commParamCount; 
        
        Function(Type ret, String name, Parameter [] parameters) { 
            this.ret = ret;
            this.name = name;
            this.parameters = parameters;
            
            commParamCount = countCommParameter();
        }

        int countCommParameter() { 
            
            int count = 0;
            
            if (parameters != null && parameters.length > 0) { 
                for (Parameter p : parameters) { 
                    if (p.type.name.equals("MPI_Comm")) { 
                        count++;
                    }
                }
            }

            return count;
        }
        
        String cleanName(String name) { 
            StringTokenizer tok = new StringTokenizer(name, " *[]");
            return tok.nextToken().trim();
        }

        void generateHeader(boolean ibis, boolean colon) { 

            if (ibis) { 
                System.out.print(ret.name + " I" + name + " ( ");
            } else {
                System.out.print(ret.name + " " + name + " ( ");
            }

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];

                System.out.print(p.type.name + " " + p.name);

                if (i != parameters.length-1) { 
                    System.out.print(", ");
                }
            }

            if (colon) { 
                System.out.println(" );");
            } else {
                System.out.println(" )");
            }
        }

        void generateTrace() { 

            // Print the trace statement
            System.out.println("#ifdef TRACE_CALLS");

            System.out.print("   INFO(0, \"" + name + "(");

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];

                if (p.pointer || p.array) { 
                    System.out.print(p.type.name + " " + p.name + "=%p");
                } else { 
                    System.out.print(p.type.name + " " + p.name + "=" + p.type.getFormat());
                }

                if (i != parameters.length-1) { 
                    System.out.print(", ");
                }
            }

            System.out.print(")\"");

            if (parameters.length > 0) { 
                System.out.print(", ");					
            }

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];

                if (p.pointer || p.array) { 
                    System.out.print(cleanName(p.name));
                } else { 
                    System.out.print(p.type.getValue(p.name));
                }

                if (i != parameters.length-1) { 
                    System.out.print(", ");
                }
            }

            System.out.println(");");			
            System.out.println("#endif // TRACE_CALLS\n");
        }

        void generateProfileHeader() {
            System.out.println("#if PROFILE_LEVEL > 0");
            System.out.println("   uint64_t profile_start, profile_end;");
            System.out.println("#endif // PROFILE_LEVEL\n");
        }
        
        void generateProfileStart() {
            System.out.println("#if PROFILE_LEVEL > 0");
            System.out.println("   profile_start = profile_start_ticks();");
            System.out.println("#endif // PROFILE_LEVEL\n");
        }
        
        Parameter find_profile_comm_parameter() {

            ArrayList<Parameter> tmp = new ArrayList<Main.Parameter>(); 
            
            for (Parameter p : parameters) { 
                if (p.type.name.equals("MPI_Comm")) {
                    tmp.add(p);
                }
            }
            
            if (tmp.size() == 0) {
                return null;
            }
            
            if (tmp.size() > 1) { 
                System.err.println("WARNING: function " + name + " has multple parameters of type MPI_Comm!");
            }
            
            for (Parameter p : tmp) {
                
                if (!p.pointer && !p.array) { 
                    return p;
                }
            }
            
            return null;
        }

        String find_profile_field() {
        
            for (StatsField s : fields) {
                if (name.equals(s.function)) {
                    return s.field;
                }
            }
            
            return "STATS_MISC";
        }
        
        void generateProfileStop() {
            System.out.println("#if PROFILE_LEVEL > 0");
            System.out.println("   profile_end = profile_stop_ticks();");

            Parameter p = find_profile_comm_parameter();
            
            if (p == null) {
                System.out.println("   profile_add_statistics(MPI_COMM_SELF, " + find_profile_field() + ", profile_end-profile_start);");   
            } else {
                System.out.println("   profile_add_statistics(" + p.name + ", " + find_profile_field() + ", profile_end-profile_start);");
            }
            
            if (name.equals("MPI_Finalize") || name.equals("MPI_Abort")) {
                System.out.println("   profile_finalize();");
            }
            
            System.out.println("#endif // PROFILE_LEVEL\n");
        }
        
        void generateCatchTypes() {
            
            boolean found = false;
            
            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];
            
                if (p.type.name.equals("MPI_Datatype")) {
                    found = true;
                    break;
                }
            }
            
            if (!found) { 
                return;
            }
            
            System.out.println("#ifdef CATCH_DERIVED_TYPES");

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];
            
                if (p.type.name.equals("MPI_Datatype")) {
                    System.out.println("   CHECK_TYPE(" + p.name + ");");
                }
            }
            
            System.out.println("#endif\n"); // ADD LATER! // CATCH_DERIVED_TYPES");

        }

        void generateF2CInterceptCall() { 

            System.out.println("#ifdef IBIS_INTERCEPT");
            System.out.print("   return I" + name + "(");

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];

                if (p.pointer || p.array) { 
                    System.out.print(cleanName(p.name));
                } else { 
                    System.out.print(p.name);
                }

                if (i != parameters.length-1) { 
                    System.out.print(", ");
                }
            }

            System.out.println(");");                   
            System.out.println("#else");
            System.out.print("   return P" + name + "(");

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];

                if (p.pointer || p.array) { 
                    System.out.print(cleanName(p.name));
                } else { 
                    System.out.print(p.name);
                }

                if (i != parameters.length-1) { 
                    System.out.print(", ");
                }
            }

            System.out.println(");");                   
            System.out.println("#endif // IBIS_INTERCEPT\n");
        }
              
        void generateInterceptCall() { 

            System.out.println("#ifdef IBIS_INTERCEPT");
            System.out.print("   int error = I" + name + "(");

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];

                if (p.pointer || p.array) { 
                    System.out.print(cleanName(p.name));
                } else { 
                    System.out.print(p.name);
                }

                if (i != parameters.length-1) { 
                    System.out.print(", ");
                }
            }

            System.out.println(");");			
            System.out.println("#else");
            System.out.print("   int error = P" + name + "(");

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];

                if (p.pointer || p.array) { 
                    System.out.print(cleanName(p.name));
                } else { 
                    System.out.print(p.name);
                }

                if (i != parameters.length-1) { 
                    System.out.print(", ");
                }
            }

            System.out.println(");");			
            System.out.println("#endif // IBIS_INTERCEPT\n");
        }

        void generateTraceErrors() {

            System.out.println("#ifdef TRACE_ERRORS");
            System.out.println("   if (error != MPI_SUCCESS) {");                       
            System.out.println("      ERROR(0, \"" + name + " failed (%d)!\", error);");                        
            System.out.println("   }");                 
            System.out.println("#endif // TRACE_ERRORS");
        }
        
        void generateErrorCall() { 

            // Print the trace statement
            System.out.print("#if __I" + name +"_FORWARD >= 1\n");

            System.out.print("   return P" + name + "(");

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];

                if (p.pointer || p.array) { 
                    System.out.print(cleanName(p.name));
                } else { 
                    System.out.print(p.name);
                }

                if (i != parameters.length-1) { 
                    System.out.print(", ");
                }
            }

            System.out.println(");");			

            System.out.println("#else");

            System.out.print("   FATAL(\"NOT IMPLEMENTED: " + name + " (");

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];

                if (p.pointer || p.array) { 
                    System.out.print(p.type.name + " " + p.name + "=%p");
                } else { 
                    System.out.print(p.type.name + " " + p.name + "=" + p.type.getFormat());
                }

                if (i != parameters.length-1) { 
                    System.out.print(", ");
                }
            }

            System.out.print(")\"");

            if (parameters.length > 0) { 
                System.out.print(", ");					
            }

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];

                if (p.pointer || p.array) { 
                    System.out.print(cleanName(p.name));
                } else { 
                    System.out.print(p.type.getValue(p.name));
                }

                if (i != parameters.length-1) { 
                    System.out.print(", ");
                }
            }

            System.out.println(");");			
            System.out.println("   return MPI_ERR_INTERN;");

            System.out.println("#endif // __I" + name +"_FORWARD");
        }

        boolean canForward() {

            for (int i=0;i<parameters.length;i++) { 

                Parameter p = parameters[i];

                if (p.type.name.equals("MPI_Comm") ||
                    p.type.name.equals("MPI_Group") ||
                    p.type.name.equals("MPI_Request") ||
                    p.type.name.equals("MPI_Op")) { 
                  
                    return false;
                }
            }

            return true;
        }

        void generate(String what) { 

            
            if (what.equals("ibis")) { 
               
                for (String s : skip) { 
                    if (name.equals(s)) { 
                        return;
                    }
                }
                
                System.out.println("#ifndef __I" + name);
                generateHeader(true, false);
                System.out.println("{");
                generateErrorCall();
                System.out.println("}");
                System.out.println("#endif // __I" + name);
                System.out.println("\n");

            } else if (what.equals("mpi")) {

                for (String s : skip) { 
                    if (name.equals(s)) { 
                        return;
                    }
                }
               
                generateHeader(false, false);
                System.out.println("{");
                
                if (name.endsWith("c2f")) {
                    generateF2CInterceptCall();
                } else if (name.endsWith("f2c")) {
                    generateF2CInterceptCall();
                } else {
                    generateProfileHeader();
                    generateTrace();
                    generateCatchTypes();
                    generateProfileStart();
                    generateInterceptCall();
                    generateProfileStop();
                    generateTraceErrors();
                    System.out.println("   return error;"); 
                }
                System.out.println("}");
                System.out.println("\n");

            } else if (what.equals("header")) {

                for (String s : headerskip) { 
                    if (name.equals(s)) { 
                        return;
                    }
                }
               
                if (canForward()) {
                    System.out.println("#define __I" + name + "_FORWARD 1");
                } else { 
                    System.out.println("#define __I" + name + "_FORWARD 0");
                }

                generateHeader(true, true);

                System.out.println();
            }
        }
    }

    private BufferedReader in;	
    private HashMap<String, Type> types = new HashMap<String, Type>();
    private LinkedList<Function> functions = new LinkedList<Function>();

    private Main(String file) {

        // Add all types that we know how to print.
        types.put("void", new Type("void", new ErrorPrinter()));
        types.put("char", new Type("char", new TypePrinter("%c")));
        types.put("int", new Type("int", new TypePrinter("%d")));
        types.put("double", new Type("double", new TypePrinter("%f")));
        types.put("MPI_Comm", new Type("MPI_Comm", new TypePrinter("%s", "comm_to_string")));
        types.put("MPI_Group", new Type("MPI_Group", new TypePrinter("%s", "group_to_string")));
        types.put("MPI_Request", new Type("MPI_Request", new TypePrinter("%s", "request_to_string")));
        types.put("MPI_Datatype", new Type("MPI_Datatype", new TypePrinter("%s", "type_to_string")));
        types.put("MPI_Op", new Type("MPI_Op", new TypePrinter("%s", "op_to_string")));
        types.put("MPI_Info", new Type("MPI_Info", new TypePrinter("%s", "info_to_string")));
        types.put("MPI_File", new Type("MPI_File", new TypePrinter("%s", "file_to_string")));
        types.put("MPI_Win", new Type("MPI_Win", new TypePrinter("%s", "win_to_string")));
        types.put("MPI_Fint", new Type("MPI_Fint", new TypePrinter("%d")));

        open(file);
    }

    private static void fatal(String error, Exception e) { 

        System.err.println(error);

        if (e != null) { 
            System.err.println(e);
            e.printStackTrace(System.err);
        }

        System.exit(1);	
    }

    private void open(String file) { 
        try {
            in = new BufferedReader(new FileReader(file));
        } catch (FileNotFoundException e) {
            fatal("Failed to open input file: " + file, e);
        }
    }

    private Type getType(String name) {

        Type type = types.get(name);

        if (type == null) {
            type = new Type(name, new UnknownTypePrinter());
            types.put(name, type);
        }

        return type;
    }

    private String replaceSensitive(String name) { 
       
        for (int i=0;i<sensitive.length;i++) { 
            // Use regexp here!
            
            // If the sensitive name matches exactly we replace it.
            if (name.equals(sensitive[i][0])) {   
                return sensitive[i][1];
            }
            
            // If the sensitive name is a substring we check futher.
            int index = name.indexOf(sensitive[i][0]);
       
            if (index > 0) { 
                
                // Is there a header of -only- 0 or more "*"'s ?
                boolean pointerHeader = true;
                
                for (int j=0;j<index;j++) {
                    if (name.charAt(j) != '*') {
                        pointerHeader = false;
                        break;
                    }
                }
           
                // Is there a trailer of -only- 0 or more array brackets ?
                boolean arrayTrailer = true;
                
                int end = index+sensitive[i][0].length();
            
                for (int j=end;j<name.length();j++) {
                    if (name.charAt(j) != '[' && name.charAt(j) != ']') {
                        arrayTrailer = false;
                        break;
                    }
                }
            
                // Only if there header and trailer are OK, we replace the string.
                if (pointerHeader && arrayTrailer) {
                    return name.substring(0, index) + sensitive[i][1] + name.substring(end);
                }
            }
        }
        
        return name;
    }
    
    private Parameter getParameter(String type, String name) { 

        boolean pointer = name.startsWith("*");
        boolean array = name.endsWith("]");		
        return new Parameter(getType(type), replaceSensitive(name), pointer, array);
    }	

    private void parse(String line) { 

        StringTokenizer tok = new StringTokenizer(line, " (),");

        Type ret = getType(tok.nextToken());

        String name = tok.nextToken();

        ArrayList<Parameter> params = new ArrayList<Parameter>();

        while (tok.hasMoreTokens()) { 
            params.add(getParameter(tok.nextToken(), tok.nextToken()));
        }

        functions.add(new Function(ret, name, params.toArray(new Parameter[params.size()])));
    }

    private String readLine() { 

        try {
            String line = in.readLine();

            if (line == null) { 
                return null;
            }

            line = line.trim();

            String tmp = in.readLine(); 

            while (tmp != null && !tmp.equals("#")) { 
                line += " " + tmp.trim();
                tmp = in.readLine();
            }

            return line;

        } catch (IOException e) {
            fatal("Parse error", e);
            return null; 
        }
    } 

    private void parse() { 

        String line = readLine();

        while (line != null) {
            try {
                parse(line);
            } catch (Exception e) {
                System.err.println("Failed to parse line: " + line);
            }
            line = readLine();
        }		
    }

    private void print(String what) { 
        
        if (what.equals("mpi")) {
            // print header
            System.out.println("#include \"flags.h\"\n");
            System.out.println("#ifdef ENABLE_INTERCEPT\n");
            System.out.println("#include \"profiling.h\"");
            System.out.println("#include \"logging.h\"");
            System.out.println("#include \"debugging.h\"");
            System.out.println("#include \"mpi.h\"");
            System.out.println("#include \"generated_header.h\"\n");
            
            System.out.println("int MPI_Init ( int *argc, char ***argv )");
            System.out.println("{");
            System.out.println("   init_debug();");
            System.out.println("   profile_init();\n");
         
            System.out.println("#ifdef TRACE_CALLS");
            System.out.println("   INFO(0, \"MPI_Init(int *argc=%p, char ***argv=%p)\", argc, argv);");
            System.out.println("#endif // TRACE_CALLS\n");

            System.out.println("#ifdef IBIS_INTERCEPT");
            System.out.println("   int error = IMPI_Init(argc, argv);");
            System.out.println("#else");
            System.out.println("   int error = PMPI_Init(argc, argv);");
            System.out.println("#endif // IBIS_INTERCEPT\n");

            System.out.println("#ifdef TRACE_ERRORS");
            System.out.println("   if (error != MPI_SUCCESS) {");
            System.out.println("      ERROR(0, \"MPI_Init failed (%d)!\", error);");
            System.out.println("   }");
            System.out.println("#endif // TRACE_ERRORS");
            System.out.println("   return error;");
            System.out.println("}\n\n");
       
        } else if (what.equals("header")) {

            System.out.println("#ifndef __GENERATED_HEADER_H_");
            System.out.println("#define __GENERATED_HEADER_H_\n");

            System.out.println("#include \"flags.h\"\n");

            System.out.println("#ifdef IBIS_INTERCEPT\n");

            System.out.println("#include \"mpi.h\"\n");
        }
        
        for (Function f : functions) { 
            f.generate(what);
        }

        if (what.equals("mpi")) {
            System.out.println("#endif // ENABLE_INTERCEPT\n");
        } else if (what.equals("header")) {
            System.out.println("#endif // IBIS_INTERCEPT\n");
            System.out.println("#endif // _GENERATED_HEADER_H_\n");
        }
    }

    private void run(String what) { 
        parse();
        print(what);
    }

    public static void main(String [] args) { 

        if (args.length != 2) {
            fatal("Usage: generator.Main <inputfile> <what>", null);
            System.exit(1);
        }

        new Main(args[0]).run(args[1]);
    }


}
