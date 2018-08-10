#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libabikeccak.h"

#include "modedictionary.h"
#include "modenonce.h"

enum Mode { DICTIONARY, NONCE, HASH };

struct arguments {
  enum Mode mode;
  char *from;
  char *fromPrefix;
  char *to;
  char *toPrefix;
  int threadCount;
};

int arguments_parse_mode(char *string, enum Mode *toBeSet) {
  if (string == NULL)
    return 0;
  else if (strcmp(string, "DICTIONARY") == 0) {
    *toBeSet = DICTIONARY;
    return 1;
  } else if (strcmp(string, "NONCE") == 0) {
    *toBeSet = NONCE;
    return 1;
  } else if (strcmp(string, "HASH") == 0) {
    *toBeSet = NONCE;
    return 1;
  } else
    return 0;
}

error_t arguments_parse(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  int parsed;
  switch (key) {
    case 'm':
      if (!arguments_parse_mode(arg, &(arguments->mode))) argp_usage(state);
      break;
    case 'f':
      arguments->fromPrefix = arg;
      break;
    case 't':
      arguments->toPrefix = arg;
      break;
    case 'c':
      parsed = atoi(arg);
      if (parsed > 0) {
        arguments->threadCount = parsed;
      } else {
        argp_usage(state);
      }
      break;
    case ARGP_KEY_ARG:
      if (state->arg_num >= 2)
        argp_usage(state);
      else if (state->arg_num == 0) {
        arguments->from = arg;
        break;
      } else if (state->arg_num == 1) {
        arguments->to = arg;
        break;
      }
      break;
    case ARGP_KEY_END:
      if (!((arguments->mode == HASH && state->arg_num == 1) ||
            (arguments->mode == NONCE && state->arg_num == 2) ||
            (arguments->mode == DICTIONARY && state->arg_num == 2)))
        argp_usage(state);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  char *argp_program_version = "SACT v1.0";
  char *argp_program_bug_address = "konstantin.fickel@student.uni-augsburg.de";
  char doc[] = "A program calculating collisions of the Solidity ABI.";

  char args_doc[] = "from to";

  struct argp_option options[] = {
      {"mode", 'm', "HASH|DICTIONARY|NONCE", 0, "Mode of execution."},
      {"from-prefix", 'f', "STRING", 0, "Prefix to be used on the from-side."},
      {"to-prefix", 't', "FILE", 0, "Prefix to be used on the to-side."},
      {"threads", 'c', "INTEGER", 0,
       "Amount of threads used when no dictionary is used at the from-side."},
      {0}};

  struct arguments arguments;
  arguments.mode = HASH;
  arguments.from = NULL;
  arguments.fromPrefix = NULL;
  arguments.to = NULL;
  arguments.toPrefix = NULL;
  arguments.threadCount = 4;

  struct argp argp = {options, arguments_parse, args_doc, doc};
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  if (arguments.mode == DICTIONARY) {
    bruteforce_mode_dictionary(arguments.from, arguments.to,
                               arguments.fromPrefix, NULL, arguments.toPrefix,
                               NULL);
    return EXIT_SUCCESS;
  } else if (arguments.mode == NONCE) {
    bruteforce_mode_nonce(
        arguments.fromPrefix == NULL ? "" : arguments.fromPrefix,
        arguments.from, arguments.toPrefix == NULL ? "" : arguments.toPrefix,
        arguments.to, arguments.threadCount);
    return EXIT_SUCCESS;
  } else if (arguments.mode == HASH) {
    char *hash;
    calculate_contract_abi_hash(arguments.from, &hash);
    printf("%s\n", hash);
    free(hash);
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}
