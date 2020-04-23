# bin2c

Convert binary files into C-code.
More precisely, it will create an array of uint8_t containing the binary data.

## Compile

```sh
cmake
make
```

## Usage

```sh
./bin2c <filename>
```
e.g.
```sh
./bin2c smb.snd
```

The output file smb.c will be created and it looks like this:

```c
/**
 * @file smb.c
 */
#include <stdint.h>

/** Size in bytes of data_smb */
#define DATA_SMB_SIZE 26401

uint8_t data_smb[] = {
    0x53, 0x4e, 0x44, 0x1a, 0x03, 0x05, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    // ...
    };
```
