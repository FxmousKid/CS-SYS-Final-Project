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
