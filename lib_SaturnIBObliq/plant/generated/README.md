# saturn_ib_stack: generated C library

obliq-2 generated this library from a block diagram model.

## Files

- `saturn_ib_stack.c`, `saturn_ib_stack.h`: the model
- `LVDA_LVDC.c`, `LVDA_LVDC.h`: segregated subsystem module
- `CMakeLists.txt`: builds a static library and a smoke test
- `smoke_main.c`: the smoke test. Only `CMakeLists.txt` compiles its contents
- `cgen-manifest.json`: the ports, the generator and the files
- `library.properties`: PlatformIO and Arduino library description

## Build with CMake

```sh
cmake -S . -B build
cmake --build build
./build/saturn_ib_stack_smoke
```

## Use with PlatformIO

Put this folder in your project's `lib` folder, and include `saturn_ib_stack.h`.

## Use from C

Call `saturn_ib_stack_seed_from_inputs` once after `saturn_ib_stack_init`, with the inputs already written. The model's saved step is 0.005 s.

```c
#include "saturn_ib_stack.h"

static saturn_ib_stack_t model;

int main(void) {
  saturn_ib_stack_init(&model, 0.005);
  model.inputs.A_z_deg = 0.0;
  saturn_ib_stack_seed_from_inputs(&model);
  for (int i = 0; i < 100; i++) {
    model.inputs.A_z_deg = 0.0; /* your input */
    saturn_ib_stack_step(&model);
    double FDAI_Roll_deg = model.outputs.FDAI_Roll_deg; /* your output */
    (void)FDAI_Roll_deg;
  }
  return 0;
}
```
