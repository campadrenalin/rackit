# Language experiments

The goal right now is to have fun making a crappy first draft, and learn a lot
for someday making a real-er version. That is why it's important to resist the
urge to make the draft very good. Rather, it should be very *exploratory* and
otherwise indefensible, or there'll never be a separation between versions.
I know me! And I do not resist the urge, certainly better than anyone else, to
turn an experiment into production code.

Language features I'm interested in. These aren't promises, except to explore:

 * Heavy use of macros
 * Stack tree for concurrency and loops
 * Central repository - "every battery included"
   * Important included batteries include language tooling:
     * Testing
     * Benchmarks
     * Formatting (fun with so many macros, sheesh)
   * gitignored vendor tree, with directory file for easily downloading vendor libs
 * Thin core of language, put most of implementation in extensions
 * Inferred type data by usage
 * Compiles to C/C++ intermediate for FFI - might make stack tree tricky though
   * No LTO crisis - everything is one big "file" with tree shaking
   * 
