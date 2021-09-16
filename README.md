
# Mysql-bruteforce

### MySQL multi-threaded brute-forcer.

cp libmysqlclient.so.20 to /usr/lib/ before running the release!

## Purpose

Brute-force a MySQL user using a wordlist file.

This fork is a revised version of 0x0mar's fast brute-force program.

Main revisions are:

+ Easier command-line usage,
+ Adaptability in parsing wordlist formats,
+ Thread number default assigned from CPU info,
+ Progress counter and stats displayed.


## OS

+ Linux


## Usage

```bash
    ./mysql-bruteforce -h <host> -u <username> -f <wordlist_file> [-t <num_threads>] [-p <port>] [-v]
```

`<host>` can be localhost (fastest), a hostname, or an IP address.

There are many wordlists available e.g. [Daniel Miessler's](https://github.com/danielmiessler/SecLists/tree/master/Passwords).

`MAX_WORD_LEN` of `50` is fine for most wordlists. However, some wordlists have borked entries (e.g. long email address). For these wordlists, increase `MAX_WORD_LEN` to `140` (or, more precisely, output of `wc -L <wordlist>` + 1), and re-compile to avoid a buffer overrun / segfault.

20 threads appears to be optimal on 4-thread CPUs.

`-v` and `-vv` can be used for verbosity output.


## Remote MySQL Connections

See [MySQL-Brute docs](https://github.com/Tinram/MySQL-Brute/blob/master/README.md).


## Executables

+ x86 64-bit
+ x86 32-bit

Download from [Releases](https://github.com/Tinram/Mysql-bruteforce/releases/latest).


## Build

If *libmysqlclient-dev* is not installed:

```bash
    make deps && make && make install
```

else:

```bash
    make && make install
```

(See [MySQL-Brute docs](https://github.com/Tinram/MySQL-Brute/blob/master/README.md) for *libmysql* requirements.)


## Authors

+ 0x0mar (original)
+ Tinram (v. 0.02)


## Credits

+ Tim Čas: EOL removal.
+ Ben Alpert: microsecond timer.
