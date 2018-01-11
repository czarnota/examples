#include <stdio.h>

int main(int argc, char **argv)
{
	const char *prog_a = argv[1];
	const char *prog_b = argv[2];
	FILE *prog_a_output = NULL;
	FILE *prog_b_input = NULL;
	unsigned char buffer[2048];
	int ret = 0;

	if (argc != 3) {
		ret = -1;
		printf("Usage: unnamed_pipe PROGRAM_0 PROGRAM_1\n");
		goto usage_err;
	}

	prog_a_output = popen(prog_a, "r");

	if (!prog_a_output) {
		ret = -2;
		goto prog_a_err;
	}

	prog_b_input = popen(prog_b, "w");

	if (!prog_b_input) {
		ret = -2;
		goto prog_b_err;
	}

	while (1) {
		size_t bytes_read = fread(buffer, 1, 2048, prog_a_output);
		size_t bytes_written = 0;

		if (bytes_read == 0) {
			break;
		}

		bytes_written = fwrite(buffer, 1, bytes_read, prog_b_input);

		if (bytes_written != bytes_read) {
			break;
		}
	}

	pclose(prog_b_input);
prog_b_err:
	pclose(prog_a_output);
prog_a_err:
usage_err:

	return ret;
}
