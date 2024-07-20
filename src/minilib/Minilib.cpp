/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Minilib.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/18 14:03:08 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/18 14:13:16 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/defines.hpp"

std::string trim(std::string str)
{
    // trim right
    size_t pos = str.find_last_not_of(" \t\n");
    str.erase(pos + 1);

    // trim left
    pos = str.find_first_not_of(" \t\n");
    str.erase(0, pos);
    return (str);
}

int strtoi(std::string number)
{
    int               result;
    std::stringstream ss;

    ss << number;
    ss >> result;
    return (result);
}