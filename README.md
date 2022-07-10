# BrainCheck-C

BrainCheck-C is a library for debugging code and verifying invariants at runtime. In most cases, you will want to optimise these checks out in production builds and the library supports that.

## Bazel Integration

To integrate with the Bazel build tool add the following to your `WORKSPACE.bazel` file:

```python
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

_BRAINCHECK_VERSION = ""
_BRAINCHECK_SHA256 = ""

http_archive(
    name = "braincheck",
    sha256 = _BRAINCHECK_SHA256,
    strip_prefix = "braincheck-c-%s" % _BRAINCHECK_VERSION,
    url="https://github.com/realityforge/braincheck-c/archive/refs/tags/v%s.tar.gz" % _BRAINCHECK_VERSION,
)
```

## API Reference

The API consists of the following functions:

* `braincheck_debug(expr)`: Emit value of expression `expr`.
* `braincheck_debug_array(expr, length)`: Emit value of expression `expr` as an array representation assuming the expression is an array of size `length`.
* `braincheck_debug_hexdump(expr, length)`: Emit value of expression `expr` as a hexdump assuming the expression is an array of size `length`.
* `braincheck_backtrace()`: Emit a backtrace starting from the line in which this macro is invoked. The macro will make a best effort to create a stack trace but if tail call optimisations, function inlining, internal linkage functions are on the stack or the program was not defined with debug symbols enable then the backtrace may be of limited value.
* `braincheck_backtrace_n(max_frame_count)`: Like `braincheck_backtrace()` but at most `max_frame_count` frames are emitted.
* `braincheck_perror(label)`: Outputs a textual description associated with the current value of `errno` in a fashion similar to the stdlib call `perror(label)`

The toolkit also provides assertions that compare values, may take a message (suffixed with `_m`) and/or may only assert the first time (prefixed with `once_`). The names of the assertion macros are as follows:

* `braincheck_assert_fail(message)`
* `braincheck_assert(v1)`
* `braincheck_assert_eq(v1, v2)`
* `braincheck_assert_neq(v1, v2)`
* `braincheck_assert_lt(v1, v2)`
* `braincheck_assert_lte(v1, v2)`
* `braincheck_assert_gt(v1, v2)`
* `braincheck_assert_gte(v1, v2)`
* `braincheck_assert_m(v1, message)`
* `braincheck_assert_eq_m(v1, v2, message)`
* `braincheck_assert_neq_m(v1, v2, message)`
* `braincheck_assert_lt_m(v1, v2, message)`
* `braincheck_assert_lte_m(v1, v2, message)`
* `braincheck_assert_gt_m(v1, v2, message)`
* `braincheck_assert_gte_m(v1, v2, message)`
* `braincheck_once_assert_fail(message)`
* `braincheck_once_assert(v1)`
* `braincheck_once_assert_eq(v1, v2)`
* `braincheck_once_assert_neq(v1, v2)`
* `braincheck_once_assert_lt(v1, v2)`
* `braincheck_once_assert_lte(v1, v2)`
* `braincheck_once_assert_gt(v1, v2)`
* `braincheck_once_assert_gte(v1, v2)`
* `braincheck_once_assert_m(v1, message)`
* `braincheck_once_assert_eq_m(v1, v2, message)`
* `braincheck_once_assert_neq_m(v1, v2, message)`
* `braincheck_once_assert_lt_m(v1, v2, message)`
* `braincheck_once_assert_lte_m(v1, v2, message)`
* `braincheck_once_assert_gt_m(v1, v2, message)`
* `braincheck_once_assert_gte_m(v1, v2, message)`

## Compile time configuration

The developer MUST define the `BRAINCHECK_PRINTF` such that it takes format string and variable arguments identical to `printf`. This will control where the debug messages are emitted.

i.e.

```c
#define BRAINCHECK_PRINTF(args...) printf(args)
#include "braincheck.h"
```

The user may also define symbols to cause parts of the BrainCheck library to be optimised away at compile time. These symbols MUST be defined prior to including the library.

* Define `BRAINCHECK_DISABLE` to optimise out all macros.
* Define `BRAINCHECK_DISABLE_PERROR` to optimise out the `braincheck_perror()` macro.
* Define `BRAINCHECK_DISABLE_BACKTRACE` to optimise out the `braincheck_backtrace()` and `braincheck_backtrace_n()` macros.
* Define `BRAINCHECK_DISABLE_DEBUG` to optimise out the `braincheck_debug(...)`, `braincheck_debug_array(...)` and `braincheck_debug_hexdump()` macros.
* Define `BRAINCHECK_DISABLE_ASSERT` to optimise out the assertion macros.
* Define `BRAINCHECK_ANSI_FORMAT` to emit messages using ansi escape codes.

## Example

A simple example program that demonstrates some of the features of BrainCheck.

```c
#define BRAINCHECK_PRINTF(args...) printf(args)
#define BRAINCHECK_ANSI_FORMAT
#include <printf.h>
#include "braincheck.h"

void func1() { braincheck_backtrace_n(2); }
void func2() { func1(); }
void func3() { func2(); }
void func4() { func3(); }

void func5() { braincheck_once_assert(0 != 0); }

int main()
{
    const char message[] = "This is a message\1\1\1\1";
    braincheck_debug_hexdump(message, sizeof(message));

    const int a = 2;
    braincheck_debug(3 * a);

    int numbers[] = { -1, 13 };
    braincheck_debug_array(numbers, 2);

    errno = EDOM;
    braincheck_perror("Some maths operation");

    func4();

    braincheck_assert_lt(1, 2);

    func5();
    // The assert in funct 5 will not issue another print as it is a once assert
    func5();

    // This next assertion will fail
    braincheck_assert_lt_m(44, 2, "44 is not less than 2 - go figure?");
    return 0;
}

```

When run this will produce output such as:

![](docs/ExampleAnsi.png)

# Contributing

GLA was released as open source so others could benefit from the project. We are thankful for any
contributions from the community. A [Code of Conduct](CODE_OF_CONDUCT.md) has been put in place and
a [Contributing](CONTRIBUTING.md) document is under development.

# License

BrainCheck-C is licensed under [Apache License, Version 2.0](LICENSE).

# Credit

* This toolkit was inspired by [eerimoq/dbg-macro](https://github.com/eerimoq/dbg-macro) which was inspired by the [std::dbg](https://doc.rust-lang.org/std/macro.dbg.html) rust macro and the C++ [sharkdp/dbg-macro](https://github.com/sharkdp/dbg-macro). It also took heavy inspiration from the java based [braincheck](https://github.com/realityforge/braincheck) library.
