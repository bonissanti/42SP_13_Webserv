/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/15 20:14:26 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/15 21:53:45 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserver.hpp"

Webserver::Webserver(char **argv){
	debugMode("<Webserver> Default Constructor called");
}

Webserver::~Webserver(){
	debugMode("<Webserver> Destructor called");
}

Webserver::Webserver(const Webserver& toCopy){
	debugMode("<Webserver> Copy Constructor called");
		*this = toCopy;
}

Webserver& Webserver::operator=(const Webserver& toCopy){
	debugMode("<Webserver> Copy Assignment Operator called");
	(void)toCopy;
	return (*this);
}

// exception 
Webserver::exception::exception(const std::string& msg) : msg(msg){};
Webserver::exception::~exception(){};
const char* Webserver::exception:: what() const throw(){
	return (msg.c_str());
}