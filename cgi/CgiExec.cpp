/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExec.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 15:20:30 by jim               #+#    #+#             */
/*   Updated: 2025/10/12 15:25:57 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiExec.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include "../console/console.hpp"

//coinstr
//todo Check for python path, what to do
CgiExec::CgiExec(const std::string& script_path, const std::string& python_path) :
	_script_path(script_path), _python_path(python_path){}

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

		setenv("REQUEST_METHOD", request->getMethod().c_str(), 1);
		if (!request->getUri().getQuery().empty()){
			setenv("QUERY_STRING", request->getUri().getQuery().c_str(), 1);
		}

		//execute python script
		execl(_python_path.c_str(), "python3", _script_path.c_str(), (char*)NULL);

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
		return 0;
	}

	console::log("[CGI] script executed succesfullz", MSG);
	return output;
}




//todo : tout
