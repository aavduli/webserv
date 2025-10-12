/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExec.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 15:20:25 by jim               #+#    #+#             */
/*   Updated: 2025/10/12 14:57:49 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIEXEC_HPP
# define CGIEXEC_HPP

# include <string>
# include "../messages/data/HttpRequest.hpp"

class CgiExec
{
	private:
		std::string _script_path;
		std::string _python_path; // todo check python nistallation


	public:
		CgiExec(const std::string& script_path, const std::string& pyhton_path);
		~CgiExec();

		std::string execute(const HttpRequest* request);

};

#endif
