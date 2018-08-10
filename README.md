# SACT v1.0
Tool for calculating hash collisions of the Solidity Contract Interface.

Part of [my bachelor's thesis](https://github.com/konstantinfickel/securityofsmartcontracts).

## Mode `HASH`

```
$ ./collision "find my hash"
9bb03d0f
```

Simple way to calculate the first four bytes of keccak-hash of a given string.

## Mode `NONCE`

```
$ ./collision --mode=NONCE --threads=4 "()" "executeSomething()"
[Thread 01] hash(A()) == f446c1d0 != d90fec61 == hash(executeSomething())
[Thread 02] hash(FPXc()) == 3796f4d4 != d90fec61 == hash(executeSomething())
[Thread 03] hash(KeuEB()) == 0412673b != d90fec61 == hash(executeSomething())
[Thread 04] hash(PtRhB()) == 794e29e8 != d90fec61 == hash(executeSomething())
...
hash(wnpWTF()) == hash(executeSomething()) == d90fec61
```

Tries all the possible values for `[from-prefix][nonce in the form of [A-Za-z]+][from]`, starting from A, until a matching hash value has been found.

Since this operation may take more an hour on regular computers, it is possible to use multiple threads using the `--threads`-option.

Can be used to obfuscate calls to functions of other contracts, or to submit different parameter types than defined by the functions if the `msg.data`-string can not be calculated manually.

## Mode `DICTIONARY`

```
$ ./collision "data/from.txt" "data/to.txt" --mode=DICTIONARY
hash("transferMyMoneyToTheAddressPlease(uint232,bool,address)") = hash("validateInputWealthInstantaneously(int256[])") = 014f3c83
hash("transferMyCashToTheAccount(uint256,bool,address)") = hash("validateInputsAndGetSzaboGently(int256[])") = 60c3ba0f
...
```

To find nice examples of hash collisions, that only contain text, this two-sided dictionary brute force attack can be used.

A dictionary in this context contains segments, which are a list of strings. The values of are dictionary are all possible values, that are created if from every segment one value is chosen, which are then concatenated in the order the segments were defined in.

**Use the smaller dictionary on the `to`-side, because this side will be stored in the memory for comparison.**

### File format
To specify the dictionary, a simple ASCII-Text file can be used. The different words can be separated by `\n`, for a new dictionary segment use a line containing `[NEXT]`.

To allow for extra comments or newlines without effect on the dictionary at the end of the file, the keyword `[END]` can be used to stop the parser from reading the file.

A possible file to define the dictionary could be:
```
Hello
[NEXT]
World
C
[NEXT]
!
[END]
```
This will result in testing the following strings:
* `Hello World!`
* `Hello C!`

## Building the tool
This tool requires `gcc` with the libraries `libkeccak`, `argp` and `uthash` to build.

To build the binary file from the sources, simply use `make`. The tool can then be started using `./collision`.

