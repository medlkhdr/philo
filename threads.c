/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlakhdar <mlakhdar@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 15:30:09 by mlakhdar          #+#    #+#             */
/*   Updated: 2025/05/09 15:31:01 by mlakhdar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philos.h"


void	*routine(void *arg)
{
	t_ph	*ph;
	int		meal_counter;

	ph = (t_ph *)arg;
	meal_counter = 0;
	while (1)
	{
		if(ph->id % 2 != 0)
			msleep(1);
		if (ph->data->nop == 1)
		{
			print_status(ph, FORK);
			die(ph);
			return (NULL);
		}
		pthread_mutex_lock(ph->mutex.stop_mutex);
		if (ph->data->stop)
		{
			pthread_mutex_unlock(ph->mutex.stop_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(ph->mutex.stop_mutex);
		pthread_mutex_lock(ph->mutex.stop_mutex);
		if (ph->data->notme != -1 && meal_counter == ph->data->notme)
		{
			ph->data->stop = true;
			pthread_mutex_unlock(ph->mutex.stop_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(ph->mutex.stop_mutex);
		eat(ph, &meal_counter);
		issleep(ph);
		think(ph);
	}
	return (NULL);
}
void	*monitor_routine(void *arg)
{
	t_ph	*ph;
	int		size;
	int		i;

	ph = (t_ph *)arg;
	size = ph->data->nop;
	while (1)
	{
		i = -1;
		while (++i < size)
		{
			pthread_mutex_lock(ph->mutex.stop_mutex);
			if (ph[i].data->stop)
			{
				pthread_mutex_unlock(ph->mutex.stop_mutex);
				return (NULL);
			}
			pthread_mutex_unlock(ph->mutex.stop_mutex);
			pthread_mutex_lock(ph[i].mutex.mutex_last_meal);
			if (time_now_ms()
				- ph[i].last_meal >= ph[i].data->ttd)
			{
				pthread_mutex_unlock(ph[i].mutex.mutex_last_meal);
				die(&ph[i]);
				pthread_mutex_lock(ph[i].mutex.stop_mutex);
				ph[i].data->stop = true;
				pthread_mutex_unlock(ph[i].mutex.stop_mutex);
				return (NULL);
			}
			pthread_mutex_unlock(ph[i].mutex.mutex_last_meal);
		}
		msleep(1);
	}
	return (NULL);
}
