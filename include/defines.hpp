/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   defines.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 20:16:53 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/19 16:17:07 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFINES_HPP
#define DEFINES_HPP

#define RED "\033[0;31m"
#define BYELLOW "\033[1;33m"
#define YELLOW "\033[0;33m"
#define PURPLE "\033[0;34m"
#define BBLUE "\033[1;34m"
#define BWHITE "\033[1;37m"
#define BLACK "\033[1;30m"
#define GREEN "\033[0;32m"
#define RESET "\033[0m"

// Macro configs
#define LISTEN 0
#define SERVER_NAME 1
#define HOST 2
#define ROOT 3
#define CLIENT_MBSIZE 4
#define ROUTE 5

// Macro route
#define AUTOINDEX 0

#define MB 1024
#define GB 1048576

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#endif