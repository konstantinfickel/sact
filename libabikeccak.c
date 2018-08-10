#include <libkeccak.h>
#include <stdlib.h>

void calculate_contract_abi_hash(char *message, char **hexsum) {
  libkeccak_spec_t spec;
  libkeccak_state_t state;
  char *hashsum;

  libkeccak_spec_sha3(&spec, 256);
  libkeccak_state_initialise(&state, &spec);
  hashsum = malloc((256 + 7) / 8);
  *hexsum = malloc((8 * 4 + 7) / 8 * 2 + 1);
  libkeccak_digest(&state, message, strlen(message), 0, "", hashsum);
  libkeccak_state_fast_destroy(&state);
  libkeccak_behex_lower(*hexsum, hashsum, (8 * 4 + 7) / 8);
  free(hashsum);
}
