# Salmon
An experimental Lisp compiler

<i> WARNING! This section describes what is goals of the project are, not what is actually present. To
see what is actually present, see the [Progress Section](#Progress). </i>

Salmon is a statically typed lisp designed to seamlessly interact with C in both directions.
It is desinged to be used in cases where a high-level lanague is wanted, but it would be otherwise cumbersome
to write an sutiable foreign function interface to a particular library or API.

It's main features are:
+ Inferred static types
+ Powerful macro system
+ No GC: memory is managed through ownership tracking
+ Automatic import of C libraries
+ Automatic export of Salmon libraries for use in C programs

The language is named after the delicious fish, and as a tribute to a similar project with 
similar goals: [Carp](https://github.com/carp-lang/Carp).

## Building
Build requirements:
+ `meson` + `ninja`

Install instructions:
``` sh
git clone --recurse-submodules https://github.com/sdilts/Salmon.git
cd Salmon
meson build
cd build
ninja
```
If you feel like running the tests, run them with `ninja test`

The executable can then be ran with `./salmon/salmon`.

## Progress

This section describes what is planned, what is implemented, and maybe what still needs to be thought out,
all of which will probably change several times before this is done.
+ [ ] The Syntax of the lanague:
  + [X] Reader macros
  + [X] string literals
  + [X] Keywords and symbols
  + [ ] different floating point types
  + [X] Packages
+ [ ] Reader:
  + [ ] Reader macros
  + [ ] String literals  
  + [ ] Keywords and symbols
  + [ ] floating point numbers
  + [ ] Packages
