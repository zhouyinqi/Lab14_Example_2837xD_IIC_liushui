# Host tests

Run `run_host_tests.ps1` to compile and execute the portable tests with host GCC.

The test source deliberately uses the `.c.host` suffix. Do not rename it to `.c`: CCS scans ordinary C files below the project root and would otherwise link the host test `main()` and its `stdio/assert` constants into the DSP firmware.

Host test binaries are written to the system temporary directory, not to this CCS project.
