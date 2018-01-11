#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
	int ret = 0;
	int fd = -1;

	fd = open("/tmp/foofifo", O_WRONLY);

	(void)argc;
	(void)argv;

	if (fd < -1) {
		ret = 1;
		goto open_err;
	}

	while (1) {
		char cmdbuf[256] = "";
		ssize_t written = 0;

		printf("@ ");

		if (1 != scanf("%s", cmdbuf)) {
			continue;
		}

		if (0 == strlen(cmdbuf)) {
			continue;
		}

		if (0 == strcmp(cmdbuf, "exit")) {
			break;
		}

		written = write(fd, cmdbuf, sizeof cmdbuf);

		if (written == 0) {
			break;
		}

		if (written < 0) {
			ret = 1;
			break;	
		}
	}

	close(fd);
open_err:
	return ret;
}
