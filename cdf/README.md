# CDF - C Development Framework

## Welcome to the C Development Framework !
Here you can find a set of (hopefully) useful libraries that can be used in any C application. The feature that distinguishes this project from any other project on the web, is the fact, that it has been implemented in the object-oriented manner. If you say "C ain't an object-oriented language", then... well you are right :). But please see this sample code and say it isn't object oriented:

```c
List * list = new(List);
call(list, add, new(String, "0"));
call(list, add, new(String, "1"));
call(list, add, new(String, "2"));
call(list, add, new(String, "3"));
call(list, add, new(String, "4"));
...
# decrease the reference counter once you don't need the object anymore
REFCDEC(list);
```

I hope you are now kind-of convinced, that C can be object-oriented. (Although I agree it isn't as object-oriented as, say, C++, Java, ...).

So enjoy !

## Why this project was even started?
Well, I like the object-oriented approach to programming. And I kind-of got used to it, so was a bit hard to me to write a piece of C code. Especially using C-strings is really pain in the neck, so I decided to create some code that would wrap C-strings and make them easier to use. And once I've started working on that, I ended up with having the String 'class', which inherits the Object class (yes, inheritance and function overriding is part of this framework).

## Main features of CDF
Below you can find a set of CDF's features:
1. support for classes
2. constructors and destructors
3. inheritance and polymorphism (virtual functions)
4. memory management through reference counting
5. standard library including such classes as: String, Map, List, Array
6. HTTP module with easy to use client and multi-threaded server (work in progress)
7. JSON module (parsing json to objects and the other way round) (work in progress)
8. py2occ (Python-to-object-oriented-c) allows transforming python-like code into CDF code (experimental)


### What do I need to use this libraries?
One of the requirements of this project is to use as few dependencies as possible. So in a way, all you need is love and a Linux machine. (Windows/Mac/AnyOtherPlatform users: sorry for that - I simply have no idea how to write code for your platform).

As for the used toolchain:
* gcc is a must (there is a certain gcc-specific extension concerning handling of varargs in macros, which I use)
* Unix sockets
* dynamic loading of libraries (dlopen and Co.)

### Getting started

Checkout the project and run the following commands:
```
make modules
make
make test
```

This will compile and link the code and then run tests - if the tests pass, you're most likely ready to go.

### What's next?
I'd suggest taking look into the examples and the test cases. And also check out the [tutorial](./doc/Tutorial.md) (work in progress).

### Footnote
You may use the CDF freely, as it is licensed under the MIT license (https://gitlab.com/samirj/cdf/blob/master/LICENSE).
However, be warned that this code may not be production-ready yet - this is still work in progress. Nevertheless you are encouraged to try it out and give feedback if you find it interesting. Just drop a line or two to the following address: `samir.jorina _at_ gmail.com`
