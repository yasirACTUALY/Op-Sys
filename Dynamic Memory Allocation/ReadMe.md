# Word Array Allocation Program
This program allocates an array of specified size n and processes a given file to extract valid words. Valid words are defined as those containing only alphabetic characters and having a minimum length of three letters. Each word is then assigned a position in the array using a hash function.

If a position in the array is already occupied, the existing word will be replaced by the new one.

## Usage
After compiling the program, use the following command to run it:

```bash
Copy code
./a.out n file_name
```
 Markup : 
              * n: The size of the array.
              * file_name: The name of the file containing the words to be processed.
#Example
```bash
Copy code
./a.out 17 lion.txt
```
This command will allocate an array of size 17 and read words from the file lion.txt.
