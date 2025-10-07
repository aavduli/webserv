/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExec.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 15:20:25 by jim               #+#    #+#             */
/*   Updated: 2025/10/07 15:30:55 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIEXEC_HPP
# define CGIEXEC_HPP

# include <string>
# include "../messages/data/HttpRequest.hpp"

class CgiExec
{
	private:

		std::string _scriptPath;
		std::string _pythonPAth;

		void setupEnvironment(const HttpRequest* request);
		std::string runPyhtonScript(const std::string& path);

	public:
		CgiExec();
		~CgiExec();

		std::string executCgiScript(const std::string& path, const HttpRequest* request);
};

#endif
