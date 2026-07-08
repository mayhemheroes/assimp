/*
 * assimp/mayhem/asan_options.c — ASan runtime option overrides.
 *
 * LeakSanitizer (LSan) is enabled by default when ASan is built with
 * -fsanitize=address on Linux.  LSan works by fork()ing the target process and
 * ptrace-scanning the child.  Mayhem's coverage-collection mode ALREADY runs the
 * target under ptrace, so when LSan tries to fork+ptrace again it fails with
 * "ERROR: LeakSanitizer: ptrace(PTRACE_ATTACH, …) failed" and the process aborts
 * — producing 0 edges even when the code path is perfectly reachable.
 *
 * The per-format fuzzers (assimp_fuzzer_3ds, _stl, _obj, etc.) call ForceFormat()
 * which unregisters and deletes all non-matching importers.  At process exit, LSan
 * finds the leftover allocations from Assimp's static importer registry and aborts
 * before Mayhem can record coverage — every format-specific target reports 0 edges.
 *
 * The fix is standard: export detect_leaks=0 via __asan_default_options so LSan is
 * compiled-in but never activated at runtime.  Any real memory-safety bugs (heap
 * overflows, use-after-free, UBSan) are still caught by the non-LSan parts of ASan.
 *
 * __asan_default_options is declared WEAK in the ASan runtime; this strong
 * definition wins at link time and is linked into every fuzzer binary via build.sh.
 */

/* Ensure a strong (non-weak) definition even in C translation units. */
const char *__asan_default_options(void)
{
    return "detect_leaks=0";
}
