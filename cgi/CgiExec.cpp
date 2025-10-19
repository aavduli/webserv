/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExec.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 15:20:30 by jim               #+#    #+#             */
/*   Updated: 2025/10/19 12:53:18 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiExec.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include "../console/console.hpp"

extern char** environ;

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

	//need to send stdin for post
	int stdin_pipefd[2];
	if (pipe(stdin_pipefd) == -1){
		console::log("[CGI] failed to create second (stdin) pipe", ERROR);
		close(pipefd[0]);
		close(pipefd[1]);
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

		close(stdin_pipefd[1]);
		dup2(stdin_pipefd[0], STDIN_FILENO);
		close(stdin_pipefd[0]);

		//change dir
		//chdir("./www/cgi-bin");

		//Setup CGI variable env
		console::log("[CGI] Executing: setenv", MSG);
		setenv("REQUEST_METHOD", request->getMethod().c_str(), 1);
		setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
		setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
		setenv("SERVER_NAME", _config->getServerName().c_str(), 1);
		std::ostringstream port;
		port << _config->getPort();
		setenv("SERVER_PORT", port.str().c_str(), 1);
		setenv("SCRIPT_NAME", request->getUri().getPath().c_str(), 1);
		setenv("PATH_INFO", "", 1);
		setenv("PATH_TRANSLATED", "", 1);

		setenv("REMOTE_ADDR", "127.0.0.1", 1); //Todo func to define true real addr ?? needed


		if (!request->getUri().getQuery().empty()){
			setenv("QUERY_STRING", request->getUri().getQuery().c_str(), 1);
		}

		//for POST method
		if (request->getMethod() == "POST"){
			std::ostringstream oss;
			oss<< request->getBody().size();
			setenv("CONTENT_LENGTH", oss.str().c_str(), 1);
			std::map<std::string, std::vector<std::string> > headers = request->getHeaders();
			std::map<std::string, std::vector<std::string> >::const_iterator ct_it = headers.find("Content-Type");
			if (ct_it != headers.end() && !ct_it->second.empty())
				setenv("CONTENT_TYPE", ct_it->second[0].c_str(), 1);
			else
				setenv("CONTENT_TYPE", "application/x-www-form-urlencoded", 1);
		}
		close(stdin_pipefd[1]);



		//POST
		//TODO multipart - plaintext
		//todo content type
		//error reposnose
		//header


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
		console::log(_script_path.c_str(), ERROR);
		char* argv[] = {(char*)"python3", (char*)_script_path.c_str(), (char*) NULL};
		//print all variable environment  DEBUG
		for (char **env = environ; *env != 0; env++){
			console::log("[CGI ENV] " + std::string(*env), MSG);
		}
		execve(_python_path.c_str(), argv, environ);


		//if exce fail
		console::log("[CGI] execl failed", ERROR);
		exit(1);
	}

	//parent proc
	close(pipefd[1]);
	close(stdin_pipefd[0]);

	if (request->getMethod() == "POST" && !request->getBody().empty()){
		write(stdin_pipefd[1], request->getBody().c_str(), request->getBody().size());}
	close(stdin_pipefd[1]);

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
