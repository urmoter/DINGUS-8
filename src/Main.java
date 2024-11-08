public class Main {
    public static void main(String[] args) {
        String bin_file = new String();
        for (String i : args) { // Check through the program's arguments
            if (i.contains(".bin")) { // Is this argument a binary file (.bin)?
                if (!(bin_file.isEmpty())) { // Do we already have a binary file?
                    throw new RuntimeException("ONLY ONE BIN FILE!"); // If so, yell at the user until they understand four words.
                } else {
                    bin_file = i; // If not, then set our binary file to this argument.
                }
            }
        }

        Cpu CPU = new Cpu(bin_file); // Cocked, Locked, and ready to Rock. Fuck, that didn't rhyme.
        CPU.start(); // Let's get this shit-show going.
    }
}
