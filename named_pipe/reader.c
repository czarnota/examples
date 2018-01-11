#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int ret = 0;
	int fd = -1;
	if (mkfifo("/tmp/foofifo", 0600)) {
		goto fifo_err;
	}

	(void)argc;
	(void)argv;


	fd = open("/tmp/foofifo", O_RDONLY);

	if (fd < 0) {
		goto open_err;
	}

	
	while (1) {
		char buffer[256] = { '\0' };
		ssize_t size = read(fd, buffer, sizeof buffer - 1);
		if (size < 0) {
			ret = 1;
			break;
		}

		if (size == 0) {
			break;
		}

		printf("%s", buffer);
	}


open_err:
	unlink("/tmp/foofifo");
fifo_err:
	return ret;
}
