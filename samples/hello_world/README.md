# Title: ZPP Hello World

## Description:
The sample starts a thread that endlessly prints out a Hello World message. The
main thread will do the same thing. The prints are guarded by a mutex to prevent
console messages getting messed up.

## Example output:

```
***** Booting Zephyr OS build v1.7.99-21379-g8ec0c692022e *****
Hello World from main tid=0x20000620
Hello World from thread tid=0x20000000
Hello World from thread tid=0x20000000
Hello World from main tid=0x20000620
Hello World from thread tid=0x20000000
Hello World from thread tid=0x20000000
Hello World from main tid=0x20000620
```
