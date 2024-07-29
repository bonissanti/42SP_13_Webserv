/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: brunrodr <brunrodr@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/29 18:12:20 by brunrodr          #+#    #+#             */
/*   Updated: 2024/07/29 19:42:57 by brunrodr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Response.hpp"

Response::Response() : _statusCode(200), _statusMessage("OK"), _body(""){

}

void Response::setStatusCode(int code)
{
	_statusCode = code;
	_statusMessage = getStatusMessage(code);
}

std::string Response::getStatusMessage(int code) const
{
	switch (code)
	{
		// talvez tenha em excesso
		case 200: return ("OK");
		case 400: return ("Bad request");
		case 403: return ("Forbidden");
		case 404: return ("Not found");
		case 405: return ("Method not allowed");
		case 500: return ("Internal server error");
		case 502: return ("Bad gateway");
		default: return ("Unknown status");
	}
}