```
0xFFFE and 0xFFFF make the starting IP location:
    (0xFFFE)(0xFFFF)

0xFFFD is the Terminal buffer

Access up-to 0xFFFF (2^16) in memory
0xFFFF +—————+
       |START| (Where to start executing.)
0xFFFD +—————+
       | RAM | (0x0100 -> 0xFFFD)
0x00FF +—————+
       |STACK| (0x0000 -> 0x00FF)
0x0000 +—————+
```