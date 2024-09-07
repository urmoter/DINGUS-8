public class Main {
    public static void main(String[] args) {
        String bin_file = new String();
        for (String i : args) {
            if (i.contains(".bin")) {
                if (!(bin_file.isEmpty())) {
                    throw new RuntimeException("ONLY ONE BIN FILE!");
                } else {
                    bin_file = i;
                }
            }
        }

        Cpu CPU = new Cpu("Test.bin");
        CPU.start();
    }
}
