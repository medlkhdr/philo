/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   activities.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feedback <feedback@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/09 15:29:09 by mlakhdar          #+#    #+#             */
/*   Updated: 2025/05/11 18:23:12 by feedback         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philos.h"

#define LOCK 1
#define UNLOCK 0

void	update_last_meal(t_ph *ph)
{
	pthread_mutex_lock(ph->mutex.mutex_last_meal);
	ph->last_meal = time_now_ms();
	pthread_mutex_unlock(ph->mutex.mutex_last_meal);
}

void	handle_forks(t_ph *ph, int action)
{
	if (action == LOCK)
	{
		if (ph->id % 2 == 0)
		{
			pthread_mutex_lock(ph->mutex.lfork);
			pthread_mutex_lock(ph->mutex.rfork);
		}
		else
		{
			pthread_mutex_lock(ph->mutex.rfork);
			pthread_mutex_lock(ph->mutex.lfork);
		}
	}
	else if (action == UNLOCK)
	{
		if (ph->id % 2 == 0)
		{
			pthread_mutex_unlock(ph->mutex.rfork);
			pthread_mutex_unlock(ph->mutex.lfork);
		}
		else
		{
			pthread_mutex_unlock(ph->mutex.lfork);
			pthread_mutex_unlock(ph->mutex.rfork);
		}
	}
}
void	eat(t_ph *ph, int *meal_counter)
{
	if (ph->id % 2 != 0)
		usleep(500);
	if (ph->data->notme != -1 && *meal_counter == ph->data->notme)
		return ;
	if (ph->data->notme != -1)
		(*meal_counter)++;
	handle_forks(ph, LOCK);
	print_status(ph, FORK);
	print_status(ph, FORK);
	update_last_meal(ph);
	print_status(ph, EAT);
	msleep(ph->data->tte);
	handle_forks(ph, UNLOCK);
}
void	issleep(t_ph *ph)
{
	msleep(ph->data->tts);
	print_status(ph, SLEEP);
}

void	think(t_ph *ph)
{
	print_status(ph, THINK);
}
void	die(t_ph *ph)
{
	msleep(ph->data->ttd);
	print_status(ph, DEATH);
}
