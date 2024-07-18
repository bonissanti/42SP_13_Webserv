/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 20:16:53 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/18 14:14:22 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_H
# define DEFINES_H

#define RED	"\033[0;31m"
#define BYELLOW	"\033[1;33m"
#define YELLOW	"\033[0;33m"
#define PURPLE	"\033[0;34m"
#define BBLUE	"\033[1;34m"
#define BWHITE	"\033[1;37m"
#define BLACK	"\033[1;30m"
#define GREEN	"\033[0;32m"
#define RESET	"\033[0m"

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sstream>
#include <exception>
#include <fstream>
#include <vector>
#include <map>
#include <string>

#include "Parser.hpp"
#include "defines.h"
#include "Server.hpp"
#include "Minilib.hpp"

#endif