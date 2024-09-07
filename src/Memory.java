import java.lang.reflect.Array;

public class Memory {
    private final int[] mem_arr = (int[]) Array.newInstance(int.class, 0x10000);

    public Memory() {
        for (int i = 0; i < 0x10000; i++) {
            mem_arr[i] = 0x00;
        }
    }

    public void write(int data, int address) {
        mem_arr[address] = data;
    }

    public int read(int address) {
        return mem_arr[address];
    }
}
