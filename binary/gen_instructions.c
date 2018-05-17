/*
 * gcc -Wall -Wextra -O2 gen_instructions.c -o gen_instructions -lcapstone
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <getopt.h>
#include <limits.h>
#include <ctype.h>
#include <inttypes.h>
#include <capstone/capstone.h>

#define MAX_INSTRUCTION_LENGTH 20
#define CHARSET "abcdefghijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"

int next_instruction(uint8_t *code, size_t length, uint8_t *charset,
		     size_t charset_length, uint8_t *charset_table)
{
  size_t i = 0;

  while (code[i] == charset[charset_length - 1] && i < length) {
    code[i] = charset[0];
    i++;
  }

  if (i == length) {
    return 0;
  }

  code[i] = charset[charset_table[code[i]] + 1];

  return 1;
}

int init_charset_table(uint8_t charset_table[256], uint8_t *charset,
			size_t charset_length)
{
  size_t i;

  memset(charset_table, 0, 256);

  for (i = 0; i < charset_length; i++) {
    charset_table[charset[i]] = i;
  }
  return 1;
}

void print_instructions(cs_insn *insn, size_t count, size_t length)
{
  size_t i, j;
  size_t size = 0;

  for (i = 0; i < count; i++) {
    size += insn[i].size;
  }

  if (size == length) {
    for (i = 0; i < count; i++) {
      for (j = 0; j < insn[i].size; j++) {
	printf("%.2x ", insn[i].bytes[j]);
      }
      printf(" ->  %-8s %s\n", insn[i].mnemonic, insn[i].op_str);
    }
  }
}

void bruteforce_instructions(csh *cs_handle, size_t length, uint8_t *charset,
			     size_t charset_length, uint8_t charset_table[256])
{
  uint8_t code[MAX_INSTRUCTION_LENGTH];
  cs_insn *insn;
  size_t count;

  memset(code, charset[0], length);

  do {
    count = cs_disasm(*cs_handle, code, length, 0, 0, &insn);

    if (count > 0) {
      if (count == 1) {
	print_instructions(insn, count, length);
      }
      cs_free(insn, count);
    }

  } while (next_instruction(code, length, charset,
			    charset_length, charset_table));
}

void usage(const char *progname, int status)
{
  printf("Usage : %s [OPTIONS]\n", progname);
  printf("\nOPTIONS\n");
  printf("   --arch, -a    <arch>     Select architecture (arm-thumb, arm,\n");
  printf("                            arm64, mips32, mips64, mips32r6, sparc,\n");
  printf("                            sysz, xcore, x86-16, x86-32, x86-64)\n");
  printf("   --length, -l  <length>   Print instructions of specified length\n");
  printf("   --charset, -c <charset>  Print instructions containing charset\n");
  exit(status);
}

int string_to_integer(const char *string, size_t *result,
		      size_t min, size_t max)
{
  long int res;
  char *endptr;

  res = strtol(string, &endptr, 0);

  if (*endptr != '\0' ||
      res == LONG_MIN ||
      res == LONG_MAX ||
      (size_t) res < min ||
      (size_t) res > max) {
    return 0;
  }

  *result = res;

  return 1;
}

int string_to_arch(const char *string, int *cs_arch, int *cs_mode)
{
  int i, array_size;
  struct arch_mode {
    const char *name;
    int arch;
    int mode;
  } arch_mode[] = {
    {"arm-thumb", CS_ARCH_ARM,   CS_MODE_THUMB},
    {"arm",       CS_ARCH_ARM,   CS_MODE_ARM},
    {"arm64",     CS_ARCH_ARM64, CS_MODE_ARM},
    {"mips32",    CS_ARCH_MIPS,  CS_MODE_MIPS32},
    {"mips64",    CS_ARCH_MIPS,  CS_MODE_MIPS64},
    {"mips32r6",  CS_ARCH_MIPS,  CS_MODE_MIPS32R6},
    {"sparc",     CS_ARCH_SPARC, 0},
    {"sysz",      CS_ARCH_SYSZ,  0},
    {"xcore",     CS_ARCH_XCORE, 0},
    {"x86-16",    CS_ARCH_X86,   CS_MODE_16},
    {"x86-32",    CS_ARCH_X86,   CS_MODE_32},
    {"x86-64",    CS_ARCH_X86,   CS_MODE_64},
  };

  array_size = (sizeof arch_mode) / (sizeof arch_mode[0]);

  for (i = 0; i < array_size; i++) {
    if (!strcmp(string, arch_mode[i].name)) {
      *cs_arch = arch_mode[i].arch;
      *cs_mode = arch_mode[i].mode;
      break;
    }
  }

  return i != array_size;
}

int hexa_to_integer(int c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return c - 'A' + 10;
}

int string_to_charset(const char *string, uint8_t **charset, size_t *length)
{
  const char *ptr;
  uint8_t *ptr_charset;

  ptr_charset = malloc(strlen(string));
  if (ptr_charset == NULL) {
    return 0;
  }

  *length = 0;
  *charset = ptr_charset;
  ptr = string;

  while (*ptr) {
    if (*ptr == '\\' && *(ptr+1) == 'x' &&
	isxdigit(*(ptr+2)) && isxdigit(*(ptr+3))) {
      *ptr_charset = hexa_to_integer(*(ptr+2)) * 16;
      *ptr_charset += hexa_to_integer(*(ptr+3));
      ptr += 4;
    } else {
      *ptr_charset = *ptr;
      ptr++;
    }
    ptr_charset++;
    (*length)++;
  }

  return *length != 0;
}

int main(int argc, char **argv)
{
  uint8_t charset_table[256], *charset = NULL;
  csh cs_handle;
  size_t length = 1, charset_length = 0, i;
  int cs_arch = CS_ARCH_X86, cs_mode = CS_MODE_64, opt;

  static struct option long_opts[] = {
    {"arch",    required_argument, NULL, 'a'},
    {"length",  required_argument, NULL, 'l'},
    {"charset", required_argument, NULL, 'c'},
    {"help",    no_argument,       NULL, 'h'},
  };

  while ((opt = getopt_long(argc, argv, "a:l:c:h", long_opts, NULL)) > 0) {
    switch (opt) {
    case 'a':
      if (!string_to_arch(optarg, &cs_arch, &cs_mode)) {
	errx(EXIT_FAILURE, "bad arch : %s", optarg);
      }
      break;
    case 'l':
      if (!string_to_integer(optarg, &length, 1, MAX_INSTRUCTION_LENGTH)) {
	errx(EXIT_FAILURE, "integer in range [1,%d] expected for --length",
	     MAX_INSTRUCTION_LENGTH);
      }
      break;
    case 'c':
      if (!string_to_charset(optarg, &charset, &charset_length)) {
	errx(EXIT_FAILURE, "failed to decode your charset");
      }
      break;
    case 'h':
      usage(argv[0], EXIT_SUCCESS);
      break;
    default:
      usage(argv[0], EXIT_FAILURE);
    }
  }

  if (charset == NULL &&
      !string_to_charset(CHARSET, &charset, &charset_length)) {
    errx(EXIT_FAILURE, "failed to initialize charset");
  }

  if (!init_charset_table(charset_table, charset, charset_length)) {
    errx(EXIT_FAILURE, "failed to initialize charset table !");
  }

  if (cs_open(cs_arch, cs_mode, &cs_handle) != CS_ERR_OK) {
    errx(EXIT_FAILURE, "failed to open Capstone Handle !");
  }

  for (i = 1; i <= length; i++) {
    bruteforce_instructions(&cs_handle, i, charset,
			    charset_length, charset_table);
  }

  cs_close(&cs_handle);
  free(charset);
  return EXIT_SUCCESS;
}
