
-This is a file encryptor/decrypter written in cpp.
-Firstly you can use python3 makeTestDir.py to generate random text files. (I'm not putting the test folder on github cause it just contains random text)
-Then run make clean && make
-To run it you gotta use ./encrypt_decrypt 
-It uses multiprocesssing and I've tested it upto 10,000 files in the folder. 
-It does get stuck at 20000+ files.
-If you do find yourself stuck and exit the program abruptly make sure to clear out the folder /dev/shm cause it contains the semaphore mmap files if you don't clear it will stop working even with same executable on smaller number of files.


