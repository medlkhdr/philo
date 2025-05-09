/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   housekeepers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlakhdar <mlakhdar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 15:31:32 by mlakhdar          #+#    #+#             */
/*   Updated: 2025/05/09 15:31:41 by mlakhdar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philos.h"

void	Housekeeping(t_housekeeped clean)
{
	int	size;
	int	i;

	i = 0;
	size = clean.ph[0].data->nop;
	while (i < size)
	{
		pthread_mutex_destroy(&clean.forks[i]);
		i++;
	}
	pthread_mutex_destroy(clean.mutex_last_meal);
	pthread_mutex_destroy(clean.stop);
	free(clean.ph);
	free(clean.thread);
	free(clean.mutex_last_meal);
	free(clean.forks);
	free(clean.print_mutex);
	free(clean.stop);
}
