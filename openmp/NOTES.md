# Memory
Memory is shared (and thus restricted to the same system).

# Common Pitfalls
### Assuming state of variables
By default, all variables within a parallel pragma are shared (which were defined out of scope). Variables you desire to not suffer from race conditions with must be explicitly defined as private.
### Believing that variables are atomic by default
Variables are not atomic, e.g. running the following code would not guarantee `r` resulting in 5.
```c
int r = 0;
#pragma omp parallel for
for (int i = 0; i < 5; ++i) {
  ++r;
}
```
To ensure atomicity (and the value of `r` being 5), you must use the `atomic` (or `critical` for a block of code) pragma.
```c
int r = 0;
#pragma omp parallel for
for (int i = 0; i < 5; ++i) {
  #pragma omp atomic
  ++r;
}
```
```c
int r = 0;
#pragma omp parallel for
for (int i = 0; i < 5; ++i) {
  #pragma omp critical
  {
    //Only one thread can enter this block of code at a time
    //Possibly useful for calling a function which
    // modifies r (e.g. func(&r))
    ++r;
  }
}
```
