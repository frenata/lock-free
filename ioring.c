#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_RING 60

typedef struct {
  int id;
  void *data;
} handle;

typedef struct {
  handle *handlers[MAX_RING];
  int slots;
  int req_prod, req_cons, res_prod, res_cons;
} ring;

typedef struct {
  int arg1, arg2;
  char op;
  int result;
} computation;

void *client(void *arg);
void *server(void *arg);

ring *ring_create(int slots) {
  ring *r = malloc(sizeof(ring));
  r->req_prod = 0;
  r->req_cons = 0;

  int mid = slots / 2;
  r->res_prod = mid;
  r->res_cons = mid;
  r->slots = slots;

  return r;
}

int main(int argc, char **argv) {
  int slots = MAX_RING;
  if (argc > 1) {
    slots = atoi(argv[1]);
    slots = slots > MAX_RING ? MAX_RING : slots;
  }
  printf("ring with %d slots\n", slots);

  ring *r = ring_create(slots);

  pthread_t client_t, server_t;

  pthread_create(&server_t, NULL, server, r);
  pthread_create(&client_t, NULL, client, r);

  pthread_join(server_t, NULL);
  pthread_join(client_t, NULL);

  free(r);

  return 0;
}

void *server(void *arg) {
  ring *r = arg;
  computation *c;
  int id = 0;
  int requests = 10;
  int responses = 10;

  for(;;) {
    /* if (!(requests + responses)) { */
    /*   return NULL; */
    /* } */

    // try to consume requests
    if (/* requests &&  */
        !id && (r->req_prod != r->req_cons)) { // request is available! seg
      handle *h = r->handlers[r->req_cons];
      c = h->data;

      fprintf(stderr, "Processing request #%d from slot %d.\n", h->id, r->req_cons);

      sleep(rand() % 4);

      switch(c->op) {
      case '+':
        c->result = c->arg1 + c->arg2;
        break;
      }

      id = h->id;
      free(h);
      r->req_cons = (r->req_cons + 1) % r->slots;
      requests--;
    }

    // try to produce responses
    if (/* responses && */
        id && (r->res_prod != r->req_cons)) {
      handle *h = malloc(sizeof(handle));
      h->id = id;
      h->data = c;

      r->handlers[r->res_prod] = h;
      fprintf(stderr, "Produced response to #%d to slot %d.\n", h->id, r->res_prod);
      r->res_prod = (r->res_prod + 1) % r->slots;
      id = 0;
      responses--;
    }
  }
}

void *client(void *arg) {
  ring *r = arg;
  int requests = 10;
  int responses = 10;

  for (;;) {
    /* if (!(requests + responses)) { */
    /*   return NULL; */
    /* } */
    // try to produce a request
    if (/* requests && */
        r->req_prod != r->res_cons) { // room for a req!
      sleep(rand() % 4);
      computation *c = malloc(sizeof(computation));
      c->op = '+';
      c->arg1 = rand();
      c->arg2 = rand();

      handle *h = malloc(sizeof(handle));
      h->data = c;
      h->id = rand(); // TODO monotonically increasing id generator

      r->handlers[r->req_prod] = h;
      fprintf(stderr, "Produced req #%d: %d %c %d = ? into slot %d\n",
              h->id, c->arg1, c->op, c->arg2, r->req_prod);
      r->req_prod = (r->req_prod + 1) % r->slots;
      requests--;
    }

    // try to consume response
    if (/* responses && */
        r->res_prod != r->res_cons) { // response is available!
      handle *h = r->handlers[r->res_cons];
      computation *c = h->data;
      fprintf(stdout, "Got response! #%d: %d %c %d = %d from slot %d\n",
              h->id, c->arg1, c->op, c->arg2, c->result, r->res_cons);
      free(c);
      free(h);
      r->res_cons = (r->res_cons + 1) % r->slots;
      responses--;
    }
  }
}
