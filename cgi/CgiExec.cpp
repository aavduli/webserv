/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExec.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 15:20:30 by jim               #+#    #+#             */
/*   Updated: 2025/10/13 15:39:10 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiExec.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include "../console/console.hpp"

//coinstr
//todo Check for python path, what to do
CgiExec::CgiExec(const std::string& script_path, const std::string& python_path, const WebservConfig* config) :
	_script_path(script_path), _python_path(python_path), _config(config){}

CgiExec::~CgiExec(){}

std::string CgiExec::execute(const HttpRequest* request){
	console::log("[CGI] Executing: " + _script_path, MSG);

	int pipefd[2];
	if (pipe(pipefd) == -1){
		console::log("[CGI] failed to create pipe", ERROR);
		return "";
	}

	//fork
	pid_t pid = fork();
	if (pid == -1){
		console::log("[CGI] Fork failed", ERROR);
		close(pipefd[0]);
		close(pipefd[1]);
		return "";
	}

	if (pid == 0){
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		//change dir
		chdir("./www/cgi");

		//Setup CGI variable env
		setenv("REQUEST_METHOD", request->getMethod().c_str(), 1);
		setenv("SERVER_PROTOCOL", "HTTP/1.0", 1);
		setenv("GATEWAY_INTERFACE", "CGI/1.0", 1);
		setenv("SERVER_NAME", _config->getServerName().c_str(), 1);
		std::ostringstream port;
		port << _config->getPort();
		setenv("SERVER_PORT", port.str().c_str(), 1);
		setenv("SCRIPT_NAME", request->getUri().getPath().c_str(), 1);

		if (!request->getUri().getQuery().empty()){
			setenv("QUERY_STRING", request->getUri().getQuery().c_str(), 1);
		}

		//for POST method
		if (request->getMethod() == "POST"){
			std::ostringstream oss;
			oss<<request->getBodySize();
			setenv("CONTENT_LENGTH", oss.str().c_str(), 1);
			setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
		}

		//HTTP heeaders (HTTP_*)
		std::map<std::string, std::vector<std::string> > headers = request->getHeaders();
		for (std::map<std::string, std::vector<std::string> >::const_iterator it = headers.begin();
			it != headers.end(); ++it){
				std::string header_name = "HTTP_" + it->first;
				//upercase and _ instead of -
				for (size_t i = 0; i < header_name.length(); i++){
					if (header_name[i] == '-')
						header_name[i] = '_';
					header_name[i] = std::toupper(header_name[i]);
				}
				if (!it->second.empty()){
					setenv(header_name.c_str(), it->second[0].c_str(), 1);
				}
			}


		//execute python script
		if (access(_python_path.c_str(), X_OK) != 0){
			console::log("[CGI] python not found: " + _python_path, ERROR);
			exit(1);
		}
		execl(_python_path.c_str(), "python3", "script.py", (char*)NULL);

		//if exce fail
		console::log("[CGI] execl failed", ERROR);
		exit(1);
	}

	//parent proc
	close(pipefd[1]);

	std::string output;
	char buffer[4096];
	size_t bytes_read;

	while((bytes_read =read(pipefd[0], buffer, sizeof(buffer))) > 0){
		output.append(buffer, bytes_read);
	}

	int status;
	waitpid(pid, &status, 0);

	if (WEXITSTATUS(status) != 0){
		console::log("[CGI] script execution failed", ERROR);
		return "";
	}

	console::log("[CGI] script executed succesfullz", MSG);
	return output;
}
