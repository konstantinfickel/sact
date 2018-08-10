#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libabikeccak.h"

#include "modenonce.h"

struct brute_force_param {
  int offset;
  int step;
  char *fromPrefix;
  char *fromPostfix;
  char *toPrefix;
  char *toPostfix;
  int *finish;
};

struct DynamicString {
  int length;
  char *content;
};

void allocateDynamicString(struct DynamicString **dynString) {
  *dynString = malloc(sizeof(struct DynamicString));
  (*dynString)->length = 1;
  (*dynString)->content = strdup("A");
}

void freeDynamicString(struct DynamicString **dynString) {
  free((*dynString)->content);
  free(*dynString);
  *dynString = NULL;
}

// Works without memory problems up to by = 52
void incrementIdentifier(struct DynamicString *dynString, int by) {
  int overflow = by;
  for (int i = 0; i < dynString->length; i++) {
    int value = dynString->content[i] >= 'a'
                    ? dynString->content[i] - 'a' + ('z' - 'a' + 1)
                    : dynString->content[i] - 'A';

    value += overflow;
    overflow = value / ('Z' - 'A' + 'z' - 'a' + 2);
    value %= ('Z' - 'A' + 'z' - 'a' + 2);

    dynString->content[i] =
        value > ('z' - 'a') ? value + 'a' - ('z' - 'a' + 1) : value + 'A';
    if (overflow == 0) {
      return;
    }
  }

  overflow--;
  dynString->content[dynString->length] = overflow > ('z' - 'a')
                                              ? overflow + 'a' - ('z' - 'a' + 1)
                                              : overflow + 'A';
  dynString->length += 1;
  dynString->content =
      realloc(dynString->content, (dynString->length + 1) * sizeof(char));
  dynString->content[dynString->length] = '\0';
}

int brute_force_thread(void *arg) {
  struct brute_force_param *param = (struct brute_force_param *)arg;

  char *targetHash, *sourceHash, sourceIdentifier[90], targetIdentifier[90];

  sprintf(targetIdentifier, "%s%s", param->toPrefix, param->toPostfix);
  calculate_contract_abi_hash(targetIdentifier, &targetHash);

  struct DynamicString *identifier;
  allocateDynamicString(&identifier);
  incrementIdentifier(identifier, param->offset);

  for (int i = param->offset; !(*(param->finish)); i += param->step) {
    sprintf(sourceIdentifier, "%s%s%s", param->fromPrefix, identifier->content,
            param->fromPostfix);
    calculate_contract_abi_hash(sourceIdentifier, &sourceHash);

    if (i % (param->step * 10000000 + 1) == 0) {
      printf("[Thread %02d] hash(%s) == %s != %s == hash(%s)\n",
             param->offset + 1, sourceIdentifier, sourceHash, targetHash,
             targetIdentifier);
    }

    if (strcmp(sourceHash, targetHash) == 0) {
      printf("hash(%s) == hash(%s) == %s\n", sourceIdentifier, targetIdentifier,
             targetHash);
      free(sourceHash);
      free(targetHash);
      *(param->finish) = 1;
      goto exit;
    }

    incrementIdentifier(identifier, param->step);
    free(sourceHash);
  }

exit:
  freeDynamicString(&identifier);
  free(targetHash);
  pthread_exit(NULL);
}

void bruteforce_mode_nonce(char *fromPrefix, char *fromPostfix, char *toPrefix,
                           char *toPostfix, int threadCount) {
  struct brute_force_param bfparam[threadCount];
  pthread_t thread_id[threadCount];

  int finish = 0;

  for (int i = 0; i < threadCount; i++) {
    bfparam[i].offset = i;
    bfparam[i].step = threadCount;
    bfparam[i].fromPrefix = fromPrefix;
    bfparam[i].fromPostfix = fromPostfix;
    bfparam[i].toPrefix = toPrefix;
    bfparam[i].toPostfix = toPostfix;
    bfparam[i].finish = &finish;

    pthread_create(&thread_id[i], NULL, (void *)brute_force_thread,
                   &bfparam[i]);
  }

  for (int i = 0; i < threadCount; i++) {
    pthread_join(thread_id[i], NULL);
  }
}
