/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feedback <feedback@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 15:30:09 by mlakhdar          #+#    #+#             */
/*   Updated: 2025/05/11 18:33:31 by feedback         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philos.h"

bool	handle_single_philosopher(t_ph *ph)
{
	if (ph->data->nop == 1)
	{
		print_status(ph, FORK);
		die(ph);
		return (true);
	}
	return (false);
}

bool	check_stop_condition(t_ph *ph)
{
	pthread_mutex_lock(ph->mutex.stop_mutex);
	if (ph->data->stop || (ph->data->notme != -1
			&& ph->count == ph->data->notme))
	{
		ph->data->stop = true;
		pthread_mutex_unlock(ph->mutex.stop_mutex);
		return (true);
	}
	pthread_mutex_unlock(ph->mutex.stop_mutex);
	return (false);
}
void	*routine(void *arg)
{
	t_ph	*ph;

	ph = (t_ph *)arg;
	while (1)
	{
		if (handle_single_philosopher(ph))
			return (NULL);
		if (check_stop_condition(ph))
			return (NULL);
		eat(ph);
		issleep(ph);
		think(ph);
	}
	return (NULL);
}
bool	check_stop(t_ph *ph, int i)
{
	pthread_mutex_lock(ph->mutex.stop_mutex);
	if (ph[i].data->stop)
	{
		pthread_mutex_unlock(ph->mutex.stop_mutex);
		return (true);
	}
	pthread_mutex_unlock(ph->mutex.stop_mutex);
	return (false);
}
bool	check_death(t_ph *ph, int i)
{
	pthread_mutex_lock(ph[i].mutex.mutex_last_meal);
	if (time_now_ms() - ph[i].last_meal > ph[i].data->ttd)
	{
		pthread_mutex_unlock(ph[i].mutex.mutex_last_meal);
		die(&ph[i]);
		pthread_mutex_lock(ph[i].mutex.stop_mutex);
		ph[i].data->stop = true;
		pthread_mutex_unlock(ph[i].mutex.stop_mutex);
		return (true);
	}
	pthread_mutex_unlock(ph[i].mutex.mutex_last_meal);
	return (false);
}
void	*monitor_routine(void *arg)
{
	t_ph *ph = (t_ph *)arg;
	int size = ph->data->nop;

	while (1)
	{
		for (int i = 0; i < size; i++)
		{
			if (check_stop(ph, i))
				return (NULL);
			if (check_death(ph, i))
				return (NULL);
		}
		usleep(500);
	}
	return (NULL);
}