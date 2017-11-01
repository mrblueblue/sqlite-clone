#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Result_t {
  struct timespec tstart;
  struct timespec tend;
  double execution_time_ms;
};
typedef struct Result_t Result;

enum MetaCommandResult_t {
  META_COMMAND_SUCCESS,
  META_COMMAND_UNRECOGNIZED_COMMAND
};
typedef enum MetaCommandResult_t MetaCommandResult;

enum PrepareResult_t {
  PREPARE_SUCCESS,
  PREPARE_UNRECOGNIZED_STATEMENT
};
typedef enum PrepareResult_t PrepareResult;

enum StatementType_t {
  STATEMENT_INSERT,
  STATEMENT_SELECT
};
typedef enum StatementType_t StatementType;

struct Statement_t {
  StatementType type;
};
typedef struct Statement_t Statement;

struct InputBuffer_t {
  char* buffer;
  size_t buffer_length;
  ssize_t input_length;
};

typedef struct InputBuffer_t InputBuffer;

InputBuffer* new_input_buffer() {
  InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
  input_buffer->buffer = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length = 0;

  return input_buffer;
};

Result* new_result() {
  Result* result = malloc(sizeof(Result));
  struct timespec tstart={0,0};
  struct timespec tend={0,0};

  result->tstart = tstart;
  result->tend = tend;

  return result;
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    exit(EXIT_SUCCESS);
  } else {
    return META_COMMAND_UNRECOGNIZED_COMMAND;
  }
}

PrepareResult prepare_statement(InputBuffer* input_buffer,
                                Statement* statement) {
  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
    return PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }

  return PREPARE_UNRECOGNIZED_STATEMENT;
}

Result* execute_statement(Statement* statement, Result* result) {
  switch (statement->type) {
    case STATEMENT_INSERT:
      printf("this is where we would insert\n");
      break;
    case STATEMENT_SELECT:
      printf("this is where we would select\n");
      break;
  }

  return result;
}

void print_prompt() {
  printf("db > ");
}

double get_execution_time (Result* result) {
  return ((double)result->tend.tv_sec + 1.0e-9*result->tend.tv_nsec) -
  ((double)result->tstart.tv_sec + 1.0e-9*result->tstart.tv_nsec);
}

void read_input(InputBuffer* input_buffer) {
  ssize_t bytes_read =
    getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

  if (bytes_read <= 0) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }

  printf("%s\n", input_buffer->buffer);

  input_buffer->input_length = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1] = 0;
}

int main(int argc, char* argv[]) {
  InputBuffer* input_buffer = new_input_buffer();
  while (true) {
    print_prompt();
    read_input(input_buffer);

    if (strncmp(input_buffer->buffer, ".", 1) == 0) {
      do_meta_command(input_buffer);
    } else {
      Statement statement;
      Result* result = new_result();
      switch (prepare_statement(input_buffer, &statement)) {
        case PREPARE_SUCCESS:
          break;
        case PREPARE_UNRECOGNIZED_STATEMENT:
          printf("Unrecognized keyword at start of %s\n",
                 input_buffer->buffer);
          continue;
      }

      clock_gettime(CLOCK_MONOTONIC, &(result->tstart));
      execute_statement(&statement, result);
      clock_gettime(CLOCK_MONOTONIC, &(result->tend));
      printf("Executed in %.5f seconds.\n", get_execution_time(result));
    }
  }
}
