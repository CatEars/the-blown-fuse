# Context

With bf_fuse we can mount and set up a directory with passthrough, log, fail, and slow files

# Run

```bash
# Run in one terminal - use debug/release to test a debug or release build
BUILD_CONFIG=debug ./test/Test002-basic-end-to-end-test/setup.sh
BUILD_CONFIG=release ./test/Test002-basic-end-to-end-test/setup.sh
# Run in another tab/terminal
./test/Test002-basic-end-to-end-test/run.sh
```

# Outcome

All checks prompted by run.sh should pass
