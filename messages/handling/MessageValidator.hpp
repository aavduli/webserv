#ifndef MESSAGEVALIDATOR_HPP
#define MESSAGEVALIDATOR_HPP

#include "../data/HttpMessage.hpp"
#include "../data/HttpRequest.hpp"
#include "../data/HttpResponse.hpp"
#include "../../config/WebservConfig.hpp"
#include "../../parsing/Parsing.hpp"

#include <sys/stat.h>

bool	is_valid_host(RequestUri *uri, const std::vector<std::string>& header_host, const std::string& config_host);
bool	is_valid_port(RequestUri *uri, const std::vector<std::string>& header_port, const std::string& config_port);
bool	is_allowed_method(const std::string& method, std::map<std::string, std::string> loc_config);
bool	is_valid_body_size(const size_t& size, const std::string& max_config);
bool	is_valid_path(RequestUri *uri, const WebservConfig& config, std::map<std::string, std::string> location);

std::string	canonicalize_path(const std::string& path);
bool		is_within_root(const std::string& resolved_path, const std::string& document_root);
bool		contains_traversal(const std::string& path);

#endif // MESSAGEVALIDATOR_HPP