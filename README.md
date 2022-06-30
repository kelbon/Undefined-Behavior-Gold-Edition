# Undefined-Behavior-Gold-Edition
Esoteric, beautiful or strange samples of C++ code

Now there are:

* shortest possibrle realization of `std::visit`
* `voidify` for functions which converts any other function to signature `void(void*)` on compile time
* `maybe<T>` which replaces the last semantic meaning of pointers in modern C++
* `unreachable_t` for writing expressions like `return IsX(b) ? foobar(b) : art::unreachable();`
* `array` realization without array(even C array)
* `union` realization without union and with less error prone interface

More description is in the implementations themselves
