## Features
- Memory as destination operands
  - Error if both source and destination are memory
  - Add examples for memory as source/desitnation
- Looks like I've forgotten that there should be commas between operands! Remember to fix examples too.
- Add `lea`, should be easy now that we have `loadEffectiveAddress`
- Improve error messages
- Add `sub`
- Smaller registers
  - Test behaviour of larger register when written as smaller
- Labels and `jmp`
- `cmp` and conditional jumps (`je`, `jg` etc)
- `call`, `ret` and `leave`

## Technical stuff
- Look at splitting up the library
  - Parser and Tokenizer not visible in official library
  - But still need to be testable
- Remove absolute paths from vcxprojs
- Remove Win32 configurations
- sln -> CMake
- CI
  - Compilation
  - Tests
  - Formatting
  - clang-tidy
  - ubsan
  - tsan
  - asan
