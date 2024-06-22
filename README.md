# image-steganography
The program uses LSB algorithm for encryption.

## Constants
[FILE] - an absolute path to the file

[MESSAGE] - if a message contains backspaces, then it should be enclosed in quotes

## Functionalities
* `-help, -h` Returns the general information about the program.
* `-info, -i [FILE]` Retrieves information about an image.
* `-check, -c [FILE] [MESSAGE]` Checks whether message can fit inside an image.
* `-encrypt, -e [FILE] [MESSAGE]` Encrypts a message into the file.
* `-decrypt, -d [FILE]` Returns a secret message from the file.

## License
[MIT](https://choosealicense.com/licenses/mit/)
