import java.lang.reflect.Array;

public class Memory {
    private final int[] mem_arr = (int[]) Array.newInstance(int.class, 0x10000);

    /**
     * Constructor for the Memory class.
     *
     * Initializes a memory array of size 65536 (0x10000) elements
     * and sets all elements to zero.
     */
    public Memory() {
        for (int i = 0; i < 0x10000; i++) {
            mem_arr[i] = 0x00;
        }
    }

    /**
     * Writes a specified integer value to a specified address in memory.
     *
     * @param data    the integer value to be written to memory
     * @param address the memory address where the data is to be written
     */
    public void write(int data, int address) {
        mem_arr[address] = data;
    }

    /**
     * Reads the data from the specified memory address.
     *
     * @param address the memory address to read data from
     * @return the data stored at the specified memory address
     */
    public int read(int address) {
        return mem_arr[address];
    }
}
