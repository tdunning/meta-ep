# Recorded step meta-mutation

The main implementation and driver is in `meta.c`

To compile
```bash
make
```

To run a bowl optimization without meta-mutation try this:
```bash
$ ./meta -e -f f1
...
195 0.5503696 -7.9588451
196 0.5503696 -7.9588451
197 0.5503696 -7.9588451
198 0.5503696 -7.9588451
199 0.5503696 -7.9588451
0 calmings
40200 evaluations
0.550369603682 is best point found
```
That isn't so good. Let's try meta-mutation with the recorded-step algorithm:
```bash
$ ./meta -r -f f1
...
194 6.8103802e-90 1.8884815e-45
195 2.702261e-90 1.8655549e-45
196 3.6756988e-91 1.4679137e-45
197 9.9804817e-92 7.0718516e-46
198 3.6771695e-92 5.3964027e-46
199 3.6771695e-92 5.3964027e-46
0 calmings
40200 evaluations
6.46793298758e-94 is best point found
```

That is, as they say in the business, much better.

