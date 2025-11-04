/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiExec.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jim <jim@student.42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/07 15:20:30 by jim               #+#    #+#             */
/*   Updated: 2025/11/04 16:31:16 by jim              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CgiExec.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <ctime>
#include <signal.h>
#include <sys/epoll.h>
#include "../console/console.hpp"
#include <fcntl.h>
#include "../server/NetworkHandler.hpp"
#define TIMEOUT_CGI 10

extern char** environ;


CgiExec::CgiExec(const std::string& script_path, const std::string& python_path, const WebservConfig* config, eventManager& em) :
	_script_path(script_path), _python_path(python_path), _config(config), _eventManager(em) {}

CgiExec::~CgiExec(){}

std::string CgiExec::execute(const HttpRequest* request){
	console::log("[CGI] Executing: " + _script_path, MSG);
	int status = 0;

	int pipefd[2];
	if (pipe(pipefd) == -1){
		console::log("[CGI] failed to create pipe", ERROR);
		return "";
	}
	NetworkHandler::makeNonblocking(pipefd[0]);
	NetworkHandler::makeNonblocking(pipefd[1]);

	int stdin_pipefd[2];
	if (pipe(stdin_pipefd) == -1){
		console::log("[CGI] failed to create second (stdin) pipe", ERROR);
		close(pipefd[0]);
		close(pipefd[1]);
		return "";
	}
	NetworkHandler::makeNonblocking(stdin_pipefd[0]);
	NetworkHandler::makeNonblocking(stdin_pipefd[1]);

	pid_t pid = fork();
	if (pid == -1){
		console::log("[CGI] Fork failed", ERROR);
		close(pipefd[0]);
		close(pipefd[1]);
		close(stdin_pipefd[0]);
		close(stdin_pipefd[1]);
		return "";
	}

	if (pid == 0){
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		close(stdin_pipefd[1]);
		dup2(stdin_pipefd[0], STDIN_FILENO);
		close(stdin_pipefd[0]);

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

		setenv("REMOTE_ADDR", "127.0.0.1", 1);


		if (!request->getUri().getQuery().empty()){
			setenv("QUERY_STRING", request->getUri().getQuery().c_str(), 1);
		}


		if (request->getMethod() == "POST"){
			std::ostringstream oss;
			oss<<request->getBody().size();
			setenv("CONTENT_LENGTH", oss.str().c_str(), 1);
			std::map<std::string, std::vector<std::string> > headers = request->getHeaders();
			std::map<std::string, std::vector<std::string> >::const_iterator ct_it = headers.find("Content-Type");
			if (ct_it != headers.end() && !ct_it->second.empty())
				setenv("CONTENT_TYPE", ct_it->second[0].c_str(), 1);
		}


		std::map<std::string, std::vector<std::string> > headers = request->getHeaders();
		for (std::map<std::string, std::vector<std::string> >::const_iterator it = headers.begin();
			it != headers.end(); ++it){
				std::string header_name = "HTTP_" + it->first;
				for (size_t i = 0; i < header_name.length(); i++){
					if (header_name[i] == '-')
						header_name[i] = '_';
					header_name[i] = std::toupper(header_name[i]);
				}
				if (!it->second.empty()){
					setenv(header_name.c_str(), it->second[0].c_str(), 1);
				}
			}

		if (access(_python_path.c_str(), X_OK) != 0){
			console::log("[CGI] python not found: " + _python_path, ERROR);
			exit(1);
		}
		console::log(_script_path.c_str(), ERROR);
		char* argv[] = {(char*)"python3", (char*)_script_path.c_str(), (char*) NULL};
		execve(_python_path.c_str(), argv, environ);

		console::log("[CGI] exceve failed: "+std::string(strerror(errno)), ERROR);
		exit(1);
	}

	close(pipefd[1]);
	close(stdin_pipefd[0]);

	_eventManager.addFd(pipefd[0], EPOLLOUT);

	if (request->getMethod() == "POST" && !request->getBody().empty()){
		std::string body = request->getBody();
		const char* data = body.c_str();
		size_t remain = body.size();

		while(remain > 0){
			ssize_t written = write(stdin_pipefd[1], data, remain);
			if (written < 0){
					console::log("[CGI] child process closed stdin (EPIPE)", ERROR);
					break;
				}
			if (written == 0){
				break;
			}
			data += written;
			remain -= written;
		}
	}
	close(stdin_pipefd[1]);

	std::string output;
	char buffer[4096];
	time_t start_time = time(NULL);

	while(true){
		if (time(NULL) - start_time >= TIMEOUT_CGI){
			console::log("[CGI] timeout reached, terminating child process", ERROR);
			if (kill(pid, 0) == 0){
				kill(pid, SIGTERM);
				console::log("[CGI] sent sigterm to process", MSG);

				time_t term_start = time(NULL);
				int result = 0;
				while(time(NULL) - term_start < 1){
					result = waitpid(pid, &status, WNOHANG);
					if (result > 0) break;
					usleep(100000);
				}

				if (result == 0 && kill(pid, 0) == 0){
					console::log("[CGI] SIGTERM failed, sending SIGKILL", ERROR);
					kill(pid, SIGKILL);
					waitpid(pid, &status, 0);
				}
			}
			close(pipefd[0]);
			_eventManager.delFd(pipefd[0]);
			return "CGI_TIMEOUT";
		}

		int result = waitpid(pid, &status, WNOHANG);
		if (result > 0){
			ssize_t bytes_read;
			while((bytes_read = read(pipefd[0], buffer, sizeof(buffer))) > 0){
				output.append(buffer, bytes_read);
			}
			break;
		}
		_eventManager.addFd(pipefd[0], EPOLLIN);
		int epoll_result = _eventManager.wait(1000);

		for (int i = 0; i < epoll_result; i++) {
			if (_eventManager[i].data.fd == pipefd[0] && (_eventManager[i].events & EPOLLIN)) {
				ssize_t bytes_read = read(pipefd[0], buffer, sizeof(buffer));
				if (bytes_read > 0){
					output.append(buffer, bytes_read);
				}
				else if (bytes_read == 0) {
					break;
				}
			}
		}
	}
	_eventManager.delFd(pipefd[0]);
	close(pipefd[0]);

	if (WIFEXITED(status)){
		if (WEXITSTATUS(status) != 0){
			console::log("[CGI] script exited with error code: "+nb_to_string(WEXITSTATUS(status)), ERROR);
			return "";
		}
	}
	else if (WIFSIGNALED(status)){
		console::log("[CGI] script killed by signal: "+nb_to_string(WTERMSIG(status)), ERROR);
		return "";
	}
	else{
		console::log("[CGI] script terminated abnormally", ERROR);
		return "";
	}

	console::log("[CGI] script executed succesfully", MSG);
	return output;
}
