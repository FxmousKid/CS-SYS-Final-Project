# include "utils/utils_timing.h"

char	*_get_day(int id)
{
	switch (id) {
	case 0: return "Dimanche";
	case 1: return "Lundi";
	case 2: return "Mardi";
	case 3: return "Mercredi";
	case 4: return "Jeudi";
	case 5: return "Vendredi";
	case 6: return "Samedi";
	default: return NULL;
	}
	return NULL;
}

void	print_timing(struct s_timing timing)
{
	printf("\n|*************************************|\n");
	printf("|************TIMING PRINT*************|\n\n");
	printf("minutes : [");
	for (int i = 0; i < 64; i++) {
		if ((timing.minutes >> i) & 1) {
			printf("- %d ", i);
		}
	}
	printf("]\n\n");
	printf("hours : [");
	for (int i = 0; i < 24; i++) {
		if ((timing.hours >> i) & 1) {
			printf("- %d ", i);
		}
	}
	printf("]\n\n");
	printf("days : [");
	for (int i = 0; i < 7; i++) {
		if ((timing.days >> i) & 1) {
			printf("- %s ", _get_day(i));
		}
	}
	printf("]\n\n");
	printf("|*************************************|\n\n");
}

static bool	mask_is_full(uint64_t mask, int limit)
{
	for (int i = 0; i < limit; ++i) {
		if (!(mask & (1ULL << i)))
			return false;
	}
	return true;
}

static void	format_field(char *dst, size_t dst_sz, uint64_t mask, int limit)
{
	bool	first = true;
	size_t	len = 0;

	if (dst_sz == 0)
		return;
	if (mask == 0) {
		snprintf(dst, dst_sz, "-");
		return;
	}
	if (mask_is_full(mask, limit)) {
		snprintf(dst, dst_sz, "*");
		return;
	}
	dst[0] = '\0';
	for (int i = 0; i < limit; ++i) {
		if (!(mask & (1ULL << i)))
			continue;
		int written = snprintf(dst + len, dst_sz - len, first ? "%d" : ",%d", i);
		if (written < 0 || (size_t)written >= dst_sz - len)
			break;
		len += (size_t)written;
		first = false;
	}
}

void	print_timing_human(struct s_timing timing)
{
	char	min_buf[256];
	char	hour_buf[64];
	char	day_buf[64];

	format_field(min_buf, sizeof(min_buf), timing.minutes, 60);
	format_field(hour_buf, sizeof(hour_buf), timing.hours, 24);
	format_field(day_buf, sizeof(day_buf), timing.days, 7);

	printf("%s %s %s\n", min_buf, hour_buf, day_buf);
}
