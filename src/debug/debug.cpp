/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 20:19:13 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/18 17:06:48 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/defines.hpp"

void debugMode(const string& msg)
{
#ifndef DEBUG
    (void)msg;
#endif

#ifdef DEBUG
    cout << BBLUE << msg << RESET << endl;
#endif
}

string binToHex(const char* digits)
{
    stringstream ss;
    unsigned int res = 0;
    while (*digits)
        res = (res << 1) | (*digits++ - '0');

    ss << hex << uppercase << res;
    return (ss.str());
}