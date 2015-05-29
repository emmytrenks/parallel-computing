# Prerequisites
On [Blue Waters](https://bluewaters.ncsa.illinois.edu/), running `module load craype-accel-nvidia35 cudatoolkit` is required to attach modules to the current login session.
###### When compiling
You must compile with `cc -h pragma=acc [...]`.
# Core functionality
Cores which are running the same task *always* run in lock step.
This means operations such as branches or jumps are *permitted* but **highly** discouraged.
There will be no issued warnings for this, as these pragmas are simply hints to the compiler saying
>Please create my kernels for me!

# Constructs
#### parallel
Many workers are created to accelerate the `parallel` region.
#### kernels
`kernels` may not branch or jump. `kernels` may also not contain other `parallel` or `kernels` regions
#### data
`data` constructs define variables which must be copied to and from the device memory before and after the region.
#### loop
`loop` constructs mark a loop which will be either split into `kernels` or other clause types depending on compiler analysis unless explicitly defined.

# Common Clauses
#### copy
`copy(variable[, variable])`

Denotes variables and arrays (denoted as `variables[start:end]`) which must be copied in and out of device memory.
#### copyin
`copyin(variable[, variable])`

Denotes variables and arrays (denoted as `variables[start:end]`) which must be copied into device memory.
#### copyout
`copyout(variable[, variable])`

Denotes variables and arrays (denoted as `variables[start:end]`) which must be copied out of device memory.

# References
Quickly refresh your knowledge using the quick references
[1](http://www.openacc.org/sites/default/files/OpenACC_API_QuickRefGuide.pdf) | [2](http://www.openacc.org/sites/default/files/213462%2010_OpenACC_API_QRG_HiRes.pdf)

Dive in and figure out what directives do using the full [specification](http://www.openacc.org/sites/default/files/OpenACC.2.0a_1.pdf)
