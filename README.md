
# Mysql-bruteforce

### MySQL multi-threaded brute-forcer.


## Purpose

Brute force a MySQL user using a wordlist file.


## OS

+ Linux


## Usage

    ./mysql -h <host> -u <username> -f <wordlist_file> [-t <num_threads>] [-p <port>] [-v]


Host can be localhost (fastest) or an IP address.

There are many wordlists available e.g. [Daniel Miessler's](https://github.com/danielmiessler/SecLists/tree/master/Passwords).

20 threads appears to be optimal on 4-thread CPUs.

`-v` and `-vv` can be used for verbose output.


## Remote MySQL Connections

See [MySQL-Brute docs](https://github.com/Tinram/MySQL-Brute/blob/master/README.md).


## Build

        make

(See [MySQL-Brute docs](https://github.com/Tinram/MySQL-Brute/blob/master/README.md) for *libmysql* requirements.)


## Authors

+ 0x0mar (original)

+ Tinram (v. 0.02)


## Credits

+ Tim Čas: EOL removal.
+ Ben Alpert: microsecond timer.
