# Context

bf_fuse consume flags that fuse also uses. This test ensures that bf_fuse passes on those flags correctly.

# Run

```bash
bf_fuse -h
bf_fuse -V
```

# Outcome

`-h` should output help for both fuse and bf_fuse
`-V` should output versions for bf_fuse together with all three fuse components
