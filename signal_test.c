/* ************************************************************************** */
/*                       .-.                       .                          */
/*                      / -'                      /                           */
/*       .  .-. .-.   -/--).--..-.  .  .-. .-.   /-.  .-._.)  (               */
/*        )/   )   )  /  /    (  |   )/   )   ) /   )(   )(    )         .    */
/*   By: '/   /   (`.'  /      `-'-.-/   /.- (.''--'`-`-'  `--':        /     */
/*                  -'            (   \  / .-._.).--..-._..  .-.  .-../ .-.   */
/*   Created: 20-01-2022  by       `-' \/ (   )/    (   )  )/   )(   / (  |   */
/*   Updated: 20-01-2022 11:57 by      /\  `-'/      `-'  '/   (  `-'-..`-'-' */
/*                                 `._;  `._;                   `-            */
/* ************************************************************************** */

#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void	my_sig_handler(int sig)
{
	if (sig == SIGUSR1)
	{
		write(STDOUT_FILENO, "I received SIGUSR\n", 19);
	}
	else if (sig == SIGINT)
	{
		write(STDOUT_FILENO, "I received SIGINT\n", 19);
	}
}
int main()
{
	signal(SIGINT, my_sig_handler);
	signal(SIGUSR1, my_sig_handler);
	write(STDOUT_FILENO, "Pouic\n", 6);
	fprintf(stdout, "PID: %d\n", getpid());
	sleep(3);
	write(STDOUT_FILENO, "Sleeped\n", 8);
	sleep(2);
}