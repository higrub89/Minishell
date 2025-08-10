#include "../inc/main_utils_h"

int init_minishell(t_struct *mini, char **envp_main)
{
    mini->envp = ft_copy_str_array(envp_main);
    if (!mini->envp)
    {
        perror("minishell: failed to copy environment");
        return (1);
    }
    mini->last_exit_status = 0;
    return (0);
}

void cleanup_minishell(t_struct *mini)
{
    if (mini->envp)
        free_str_array(mini->envp);
}

void free_token_list