#include "ResponseGenerator.hpp"

ResponseGenerator::ResponseGenerator(const WebservConfig& config, HttpRequest* request, HttpResponse* response, Status status) : _config(config), _request(request), _response(response), _last_status(status) {}
ResponseGenerator::ResponseGenerator(const ResponseGenerator& rhs) : _config(rhs._config), _request(rhs._request), _response(rhs._response), _last_status(rhs._last_status){}
ResponseGenerator& ResponseGenerator::operator=(const ResponseGenerator& rhs) {
	if (this != &rhs) {
		_request = rhs._request;
		_response = rhs._response;
		_last_status = rhs._last_status;
	}
	return *this;
}
ResponseGenerator::~ResponseGenerator() {}
Status	ResponseGenerator::getLastStatus() const {return _last_status;}

void ResponseGenerator::generateResponse() {

	if (_last_status == E_REDIRECT_PERMANENT || _last_status == E_REDIRECT_TEMPORARY)
		generateRedirResponse();
	else if (_last_status == E_OK && isValidCGI())
		generateCGIResponse();
	else if (_last_status == E_OK && _request->getMethod() == "POST")
		generatePostResponse();
	else if (_last_status == E_OK && _request->getMethod() == "DELETE")
		generateDeleteResponse();
	else if (_last_status == E_OK) {
		addValidIndex();
		if (is_directory(_request->getUri().getEffectivePath()))
			generateDirectoryResponse();
		else
			generateStaticFileResponse();
	}
	else
		generateErrorResponse();
	setHeaders();
}

// TODO check if correct logic
void ResponseGenerator::generatePostResponse() {

	std::string content;
	const std::map<std::string, PostData>& post_data = _request->getPostData();
	if (!post_data.empty()) {
		content = "<p>";
		for (std::map<std::string, PostData>::const_iterator it = post_data.begin(); it != post_data.end(); ++it) {
			content += "<strong>" + it->first + ":</strong> ";
			if (it->second.is_file)
				content += it->second.new_filename;
			else
				content += it->second.content;
			content += "<br>";
		}
	}

	HTMLTemplate tmpl("Success", "POST Method", content, "", "", false);
	_response->setStatus(E_OK);
	_response->setBody(tmpl.render());
	_response->setBodyType(B_HTML);
}

// TODO check if correct logic
void ResponseGenerator::generateDeleteResponse() {

	HTMLTemplate tmpl("Success", "DELETE Method", "<p>File was successfully removed.</p>", "", "", false);
	_response->setStatus(E_OK);
	_response->setBody(tmpl.render());
	_response->setBodyType(B_HTML);
}

void ResponseGenerator::generateStaticFileResponse() {

	const std::string& path = _request->getUri().getEffectivePath();

	std::ifstream file(path.c_str());
	if (!file.is_open()) {
		console::log("[ERROR][GENERATE RESPONSE] Failed to open: " + path, MSG);
		_last_status = findErrorStatus(path);
		return generateErrorResponse();
	}
	_response->setBody(get_read_file_content(file));
	file.close();
	_response->setBodyType(B_FILE);
	_response->setStatus(E_OK);
	console::log("[INFO][GET] Static file				OK", MSG);
}

void ResponseGenerator::generateDirectoryResponse() {

	if (!_request->ctx._autoindex_enabled) {
		console::log("[ERROR][GENERATE RESPONSE] Autoindex off", MSG);
		_last_status = E_FORBIDDEN;
		return generateErrorResponse();
	}
	const std::string& dir_path = _request->getUri().getEffectivePath();
	DIR* dir = opendir(dir_path.c_str());
	if (!dir) {
		console::log("[ERROR][GENERATE RESPONSE] Couldn't open directory", MSG);
		_last_status = findErrorStatus(dir_path);
		return generateErrorResponse();
	}

	const std::string& url_path = _request->getUri().getPath();
	std::string content;

	if (url_path != "/" && url_path.length() > 1) {					// if not root, link parent dir
		std::string parent_path = remove_suffix(url_path, "/");		// remove trailing /
		size_t last_slash = parent_path.find_last_of('/');
		if (last_slash == 0)
			parent_path = "/";
		else
			parent_path = parent_path.substr(0, last_slash);
		content = "<p class=\"dir-link\"><a href=\"" + parent_path + "\">../</a>\n</p>";
	}

	struct dirent *en;
	while ((en = readdir(dir)) != NULL) {
		std::string name = en->d_name;
		if (name == "." || name == "..")
			continue;
		std::string file_path = build_full_path(url_path, name);
		content += "<p class=\"dir-link\"><a href=\"" + file_path + "\">" + name + "</a>\n</p>";
	}
	closedir(dir);

	HTMLTemplate tmpl("Directory Listing", "Index of " + _request->ctx._location_name, content, "", "", false);
	_response->setBody(tmpl.render());
	_response->setBodyType(B_HTML);
	_response->setStatus(E_OK);
	console::log("[INFO][GET] Directory listing			OK", MSG);
}

void ResponseGenerator::generateRedirResponse() {

	const std::string& destination = _request->getUri().getRedirDestination();
	_response->addHeader("Location", str_to_vect(destination, ""));

	std::string title = nb_to_string(_last_status);
	std::string subtitle;
	if (_last_status == E_REDIRECT_PERMANENT)
		subtitle = "Moved Permanently";
	else
		subtitle = "Found (Moved Temporarily)";
	std::string content = "<p>The document has moved to: <a href=\"" + destination + "\">" + destination + "</a></p>\n";
	HTMLTemplate tmpl(title, subtitle, content, "", "", false);

	_response->setStatus(_last_status);
	_response->setBody(tmpl.render());
	_response->setBodyType(B_HTML);
	console::log("[INFO][GET] Redirection				OK", MSG);
}

void ResponseGenerator::generateErrorResponse() {

	_response->setStatus(_last_status);
	std::string error_page_path = _config.getErrorPage(_last_status);
	if (!error_page_path.empty()) {
		if (is_valid_file_path(error_page_path)) {
			std::ifstream file(error_page_path.c_str());
			if (file.is_open()) {
				std::string content = get_read_file_content(file);
				_response->setBody(content);
				file.close();
				_response->setBodyType(B_HTML);
				console::log("[INFO][RESPONSE] Custom error page	OK", MSG);
				return ;
			}
		}
		console::log("[ERROR][GENERATE RESPONSE] Invalid custom error page: " + error_page_path, MSG);
	}

	HTMLTemplate tmpl(nb_to_string(_last_status), status_msg(_last_status), "", "error", "error-page", false);
	_response->setBody(tmpl.render());
	_response->setBodyType(B_HTML);
	console::log("[INFO][RESPONSE] Default error page	OK", MSG);
}

void ResponseGenerator::generateCGIResponse() {
	console::log("[INFO] Generating CGI REsponse", MSG);
	const std::string& script_path = _request->getUri().getEffectivePath();
	std::string python_path = _config.getCgiPath(_request->ctx._location_name);

	CgiExec executor(script_path, python_path, &_config);
	CgiResult cgi_result = executor.startCgi(_request);
	//std::string cgi_output = executor.execute(_request);

	extern eventProcessor* g_eventProcessor;
	extern int g_clientFd;

	if (cgi_result.success && g_eventProcessor){
		g_eventProcessor->handleCgiStart(g_clientFd, cgi_result);

		console::log("[CGI] Transfered to event loop", MSG);
	}
	else{
		console::log("[ERROR] [CGI] CGI start failed", ERROR);
		_last_status = E_INTERNAL_SERVER_ERROR;
		return generateErrorResponse();
	}

	_response->setStatus(E_OK);
	_response->setBodyType(B_CGI);
}

void ResponseGenerator::parseCGIOutput(const std::string& cgi_output){
	//first find the empty lime, separator header body
	size_t header_end= cgi_output.find("\n\n");
	if (header_end == std::string::npos){
		//no head ? :(
		_response->setBody(cgi_output);
		_response->setBodyType(B_CGI);
		return;
	}

	//separator header body
	std::string header_part = cgi_output.substr(0,header_end);
	std::string body_part = cgi_output.substr(header_end + 2); // count +2 for \n\n

	//parse header line by line
	std::istringstream header_stream(header_part);
	std::string line;
	while(std::getline(header_stream, line)){
		size_t colon_position = line.find(':');
		if (colon_position != std::string::npos){
			std::string header_name = line.substr(0, colon_position);
			std::string header_value = line.substr(colon_position + 1);

			//delet space
			while (!header_value.empty() && header_value[0] == ' '){
				header_value = header_value.substr(1);}

			_response->addHeader(header_name, str_to_vect(header_value, ""));
		}
	}
	_response->setBody(body_part);
}

std::string HTMLTemplate::render() const {

	std::stringstream html;
	html << "<!DOCTYPE html>\n";
	html << "<html><head>";
	html << "<title>" << title << "</title>";
	html << "<link rel=\"stylesheet\" href=\"/css/style.css\">";
	html << "</head>\n";

	html << "<body";
	if (!page_type.empty())
		html << " class=\"" + page_type + "\"";
	html << ">\n";

	if (has_main_title) {
		html << "\t<div class=\"main-title\">\n";
		html << "\t\t<h1>" << title << "</h1>\n";
		if (!subtitle.empty())
			html << "\t\t<p class=\"subtitle\">" << subtitle << "</p>\n";
		html << "\t</div>\n\n";
	}

	html << "\t<div class=\"card\">\n";
	html << "\t\t<div class=\"inner";
	if (!card_type.empty())
		html << " " + card_type;
	html << "\">\n";

	if (!has_main_title) {
		html << "\t\t\t<h1>" << title << "</h1>\n";
		if (!subtitle.empty())
			html << "\t\t\t<p class=\"subtitle\">" << subtitle << "</p>\n";
	}

	if (!content.empty())
		html << "<br>" << content << "<br>";

	html << "\t\t<div class=\"links\">\n";
	html << "\t\t\t<a href=\"/\">Back to homepage</a>\n";
	html << "\t\t</div>\n";
	html << "\t</div>\n";
	html << "\t</div>\n";
	html << "</body></html>";
	return html.str();
}

void ResponseGenerator::setHeaders() {

	_response->addHeader("Date", str_to_vect(getCurrentGMTDate(), ""));
	_response->addHeader("Connection", str_to_vect("close", ""));

	if (_response->getBodyType() == B_FILE) {
		std::string extension = get_file_extension(_request->getUri().getEffectivePath());
		std::string content_type = getMimeType(extension);
		_response->addHeader("Content-Type", str_to_vect(content_type, ""));
	}
	else if (_response->getBodyType() == B_HTML)
		_response->addHeader("Content-Type", str_to_vect("text/html", ""));
	else if (_response->getBodyType() == B_JSON)
		_response->addHeader("Content-Type", str_to_vect("application/json", ""));
	else if (_response->getBodyType() == B_CGI)		// CGI scripts set their own Content-Type header, don't override
		console::log("[INFO] CGI response - Content-Type set by script", MSG);

	if (_response->getBodyType() == B_EMPTY)
		_response->addHeader("Content-Length", str_to_vect("0", ""));
	else {
		std::string body_size = nb_to_string(_response->getBody().size());
		_response->addHeader("Content-Length", str_to_vect(body_size, ""));
	}
}

void	ResponseGenerator::addValidIndex() {

	std::string path = _request->getUri().getEffectivePath();
	if (!is_directory(path) || _request->ctx._index_list.empty())
		return ;

	const std::vector<std::string>& indexes = _request->ctx._index_list;
	std::vector<std::string>::const_iterator it;
	for (it = indexes.begin(); it != indexes.end(); it++) {

		std::string full_index_path = build_full_path(path, *it);
		if (is_valid_file_path(full_index_path)) {
			RequestUri uri(_request->getUri());
			uri.setEffectivePath(full_index_path);
			_request->setUri(uri);
			return ;
		}
	}
}

bool	ResponseGenerator::isValidCGI() const {

	std::string path = _request->getUri().getEffectivePath();
	if (path.empty() || !is_valid_file_path(path))
		return false;

	std::map<std::string, std::string> config = _request->ctx._location_config;
	std::string cgi_extensions = config["cgi_ext"];
	if (cgi_extensions.empty())
		return false;

	std::string extension = get_file_extension(path);
	if (!extension.empty() && extension[0] != '.')
		extension = "." + extension;

	std::vector<std::string> valid_cgi_extensions = str_to_vect(cgi_extensions, " ");
	for (size_t i = 0; i < valid_cgi_extensions.size(); i++) {
		if (extension == valid_cgi_extensions[i])
			return true;
	}
	return false;
}

Status	findErrorStatus(const std::string& path) {

	struct stat file_stat;

	// stat failed = invalid path
	if (stat(path.c_str(), &file_stat) != 0) {
		if (errno == ENOENT)
			return E_NOT_FOUND;					// 404 not found
		else if (errno == EACCES)
			return E_FORBIDDEN;					// 403 permission denied
		else
			return E_INTERNAL_SERVER_ERROR;		// 500 other error
	}

	// valid path
	if (S_ISREG(file_stat.st_mode)) {			// path valid, check
		if (!(file_stat.st_mode & S_IRUSR))		// file
			return E_FORBIDDEN; 				// 403 can't read
	}
	else if (S_ISDIR(file_stat.st_mode)) {		// directory
		if (!(file_stat.st_mode & S_IRUSR))		// autoindex on
			return E_FORBIDDEN;					// 403 can't read
	}
	return E_OK;
}

std::string getMimeType(const std::string& extension) {

	// Text types
	if (extension == "html" || extension == "htm") {
		return "text/html";
	}
	if (extension == "css") {
		return "text/css";
	}
	if (extension == "js") {
		return "application/javascript";
	}
	if (extension == "txt") {
		return "text/plain";
	}
	if (extension == "json") {
		return "application/json";
	}
	if (extension == "xml") {
		return "application/xml";
	}

	// Image types
	if (extension == "png") {
		return "image/png";
	}
	if (extension == "jpg" || extension == "jpeg") {
		return "image/jpeg";
	}
	if (extension == "gif") {
		return "image/gif";
	}
	if (extension == "svg") {
		return "image/svg+xml";
	}
	if (extension == "ico") {
		return "image/x-icon";
	}
	if (extension == "webp") {
		return "image/webp";
	}

	// Font types
	if (extension == "woff") {
		return "font/woff";
	}
	if (extension == "woff2") {
		return "font/woff2";
	}
	if (extension == "ttf") {
		return "font/ttf";
	}
	if (extension == "eot") {
		return "application/vnd.ms-fontobject";
	}

	// Document types
	if (extension == "pdf") {
		return "application/pdf";
	}
	if (extension == "doc") {
		return "application/msword";
	}
	if (extension == "docx") {
		return "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	}

	// Archive types
	if (extension == "zip") {
		return "application/zip";
	}
	if (extension == "tar") {
		return "application/x-tar";
	}
	if (extension == "gz") {
		return "application/gzip";
	}

	// Default for unknown extensions
	return "application/octet-stream";
}

std::string getCurrentGMTDate() {

	time_t now;
	struct tm* gmt;
	char buf[100];

	time(&now);				// get current time
	gmt = gmtime(&now);		// convert to GMT/UTC

	// %2d:%02d (gmt->tm_hour+2)%24, gmt->tm_min))

	// format: Sun, 06 Nov 1994 08:49:37 GMT
	strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M GMT", gmt);

	std::string date(buf);
	return date;
}
