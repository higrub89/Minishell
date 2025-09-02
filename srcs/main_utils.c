#include "../inc/main_utils.h"
#include "../inc/env_utils.h"

static int is_all_digits(const char *s)
{
    if (!s || !*s)
        return (0);
    while (*s)
    {
        if (!ft_isdigit(*s))
            return (0);
        s++;
    }
    return (1);
}

static void manage_shlvl(t_struct *mini)
{
    char *current_shlvl_str;
    int shlvl_num;
    char *new_shlvl_val;

    current_shlvl_str = get_env_value("SHLVL", mini);
    shlvl_num = 1;
    if (current_shlvl_str && is_all_digits(current_shlvl_str))
    {
        shlvl_num = ft_atoi(current_shlvl_str) + 1;
        if (shlvl_num < 1)
            shlvl_num = 1;
    }
    free(current_shlvl_str);
    new_shlvl_val = ft_itoa(shlvl_num);
    if (new_shlvl_val)
    {
        ft_setenv_var(mini, "SHLVL", new_shlvl_val);
        free(new_shlvl_val);
    }
}

int init_minishell(t_struct *mini, char **envp_main)
{
    mini->envp = ft_copy_str_array(envp_main);
    if (!mini->envp)
    {
        perror("minishell: failed to copy environment");
        return (1);
    }
    manage_shlvl(mini);
    mini->last_exit_status = 0;
    mini->export_list = NULL;
    mini->should_exit = false;
    return (0);
}

void cleanup_minishell(t_struct *mini)
{
    rl_clear_history(); // Limpiar historial de readline
    if (mini && mini->envp)
    {
        free_str_array(mini->envp);
        mini->envp = NULL;
    }
    if (mini && mini->export_list)
    {
        free_str_array(mini->export_list);
        mini->export_list = NULL;
    }
}

int ft_str_is_whitespace(const char *s)
{
    if (!s)
        return (1);
    while (*s)
    {
        if (!ft_isspace(*s))
            return (0);
        s++;
    }
    return (1);
}
