# include "parser/parse_timing.h"

bool	parse_timing(struct s_task *task) 
{
	char		buf[PATH_MAX + 1] = {0};
	char		timing[13] = {0};
	int		fd;

	task->timing.minutes = 0;
	task->timing.hours = 0;
	task->timing.days = 0;

	snprintf(buf, 7 + strlen(task->path), "%stiming", task->path);
	printf("path task : %s\n", buf);

	fd = open(buf, O_RDONLY);
	if (fd <= 0) {
		ERR_MSG("fail open");
		return false;
	}

	if (read(fd, timing, 13) < 13) {
		ERR_MSG("fail read");
		return false;
	}
	memcpy(&task->timing.minutes, timing, 8);

	memcpy(&task->timing.hours, &timing[8], 4);

	task->timing.days = (uint8_t)timing[12];

	printf("minutes: %lx hours: %x days: %x\n", task->timing.minutes, task->timing.hours, task->timing.days);
	close(fd);

	print_timing(task->timing);

	return true;
}

void	test_timing_exemple()
{
	struct s_timing t;
	t.minutes = 0x00002000000007F0;
	t.hours = 0x00041100;
	t.days = 0x5C;

	printf("EXEMPLE TIMING :\n");
	print_timing(t);
}