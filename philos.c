#include "philos.h"
unsigned long tol(char *string)
{
	int		i;
	unsigned long	res;

	res = 0;
	i = 0;
	if (string[i] == '-')
	{
		printf("NO NEGATIVE NUMBERS PLEASE!\n");
		printf("THE NUMBER YOU ENTERED IS INVALID IN MY PROJECT : %s", string);
    return -1;
  }
  if (string[i] == '+')
		i++;
	while (string[i] >= '0' && string[i] <= '9')
	{
		if (res > LIMIT)
		{
			printf("overflow !!! \n");
      return -1;
    }
    res = res * 10 + (string[i] - '0');
		i++;
	}
	if (string[i] != '\0')
	{
		printf("INVALID PARAM: %s\n", string);
    return -1;
  }
  return (res);
}



int main(int ac , char **av)
{
  t_data	data;
  if(ac != 5 && ac != 6)
  {
    printf("TRY TO ENTER THE RIGHT ARGUMENTS HONEY :| ");
    return 1;
  }

  data.nop = tol(av[1]);
  data.ttd = tol(av[2]);
  data.tte = tol(av[3]);
  data.tts = tol(av[4]);
  if(ac == 6)
    data.notme = tol(av[5]);
  else
    data.notme = -1;  
  data.start_time = time_now_ms();
  init( &data);
  return (0);
}