/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: keitanig <keitanig@student.42tokyo.jp>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/09 11:51:17 by keitanig          #+#    #+#             */
/*   Updated: 2022/04/09 12:42:15 by keitanig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <stdlib.h>

void	error(int pid, char *str)
{
	if (str)
		free(str);
	printf("server: unexpected error.\n");
	kill(pid, SIGUSR2);
	exit(EXIT_FAILURE);
}

char	*make_first_str(char c)
{
	char	*first;

	first = (char *)malloc(sizeof(char) * 2);
	if (!first)
		return (NULL);
	first[0] = c;
	first[1] = '\0';
	return (first);
}

char	*add_char_to_str(char *str, char c)
{
	char	*new;
	int		i;

	if (!c)
		return (NULL);
	if (!str)
		return (make_first_str(c));
	new = (char *)malloc(sizeof(char) * strlen(str) + 2);
	if (!new)
	{
		// free(str);
		return (NULL);
	}
	i = -1;
	while (str[++i])
		new[i] = str[i];
	// free(str);
	new[i] = c;
	new[++i] = '\0';
	return (new);
}

void	handler_sigusr(int signum, siginfo_t *info, void *context)
{
	static char	c = 0xFF;
	static int	bits = 0;
	static int	pid = 0;
	static char	*message = 0;

	(void)context;
	if (info->si_pid)
		pid = info->si_pid;
	if (signum == SIGUSR1)
		c ^= (0x80 >> bits);
	else if (signum == SIGUSR2)
		c |= (0x80 >> bits);
	if (++bits == 8)
	{
		if (c)
			message = add_char_to_str(message, c);
		else
		{
			printf("%s\n", message);
			free(message);
		}
		bits = 0;
		c = 0xFF;
	}
	if (kill(pid, SIGUSR1) == -1)
		error(pid, message);
}

int	main(void)
{
	pid_t	pid;
	struct sigaction	sa;
	sigset_t	block_mask;

	sigemptyset(&block_mask);
	sigaddset(&block_mask, SIGINT);
	sigaddset(&block_mask, SIGQUIT);
	sa.sa_handler = 0;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_mask = block_mask;
	sa.sa_sigaction = handler_sigusr;
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	pid = getpid();
	printf("[minitalk server] launched on PID = %d\n", pid);
	while (1)
		pause();
}