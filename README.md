## IO Ring

A small experiment with lock free algorithms in C.

### Next Steps

 * remove pthread references
 * parameterize the ring creation
 * make the computation more interesting, with different ops
 * abstract out the core algorithm from the computation
   * should end up with some library code for a ring buffer
   * and some application code that exercises the library

### Bigger Steps

 * is this possible in general with multiple consumers? multiple producers?
 * investigate similar algorithms
 * allow the server to queue up processed responses and put them into the buffer when space is available, rather than the current approach which just blocks on a single response
