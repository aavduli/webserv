# TODO List

## ./config/ConfigParser.cpp

- **Ligne 50**: `//TODO loc`
- **Ligne 89**: `//TODO : parse locations`

## ./config/ConfigValidator.cpp

- **Ligne 43**: `bool ConfigValidator::validateBraces(const std::vector<std::string>& lines){ //todo correct this func`
- **Ligne 78**: `bool ConfigValidator::validateSyntaxe(const std::vector<std::string>& lines){//todo correct this func`
- **Ligne 115**: `void ConfigValidator::setError(const std::string& error){} // TODO`
- **Ligne 248**: `if (codeNum < 400 || codeNum > 599){ // Todo check error code min max, and if we're gonne use them all`
- **Ligne 262**: `for (std::map<std::string, std::string>::const_iterator it = config.locations.begin(); //todo add locationS to config.`
- **Ligne 302**: `setError("should we accept empty method"+ methods ); //todo maybe accept some empty methods`
- **Ligne 309**: `if (m != "GET" && m != "POST" && m != "DELETE" ){ // todo can add more if bonuses`
- **Ligne 319**: `setError("Invalid client max body size format: " +size); //todo ask bebou for MBS`
- **Ligne 324**: `if (sizeNum <= 0 || sizeNum > 1000000000){ //todo decrease bc 1 gb is kinda overkill see commetn above`
- **Ligne 347**: `bool ConfigValidator::validateTimeout(const std::string& timeout){} //todo`
- **Ligne 350**: `bool ConfigValidator::validateRedirection(const std::string& redir){} //todo`
- **Ligne 352**: `bool ConfigValidator::validateIndex(const std::string& index){} // todo`
- **Ligne 355**: `bool ConfigValidator::validatePath(const std::string& path){} //todo`
- **Ligne 357**: `bool ConfigValidator::isPortUsed(int port) const{} // todo need a free port ?`

## ./config/ConfigValidator.hpp

- **Ligne 23**: `static const size_t MAX_DIRECTIVE_LEN = 1024; // todo to change?delete?`

## ./messages/HttpHeaders.cpp

- **Ligne 38**: `std::string					new_key;	// TODO PARSING -> extract key from string`
- **Ligne 39**: `std::vector<std::string>	new_value;	// TODO PARSING -> extract value from string`
- **Ligne 41**: `// TODO PARSING -> check for multiple comma-separated strings in new_value`
- **Ligne 59**: `// TODO -> add checks`
- **Ligne 72**: `// TODO`

## ./messages/HttpMessage.cpp

- **Ligne 50**: `// TODO PARSING -> fields parsed from buffer`

## ./messages/HttpRequest.cpp

- **Ligne 170**: `// TODO: Implement header parsing`
- **Ligne 173**: `std::cout << "Parsing headers (TODO)" << std::endl;`
- **Ligne 180**: `// TODO: Implement body parsing based on Content-Length or Transfer-Encoding`
- **Ligne 182**: `std::cout << "Parsing body (TODO)" << std::endl;`

## ./messages/HttpRequest.hpp

- **Ligne 27**: `TODO: Parse incoming HTTP request from buffer`
- **Ligne 30**: `TODO: Create HTTP response with status code, headers and output body if needed`
## TODO List

Liste de tous les TODOs trouvés dans les fichiers .cpp/.hpp

### ./config/ConfigParser.cpp

- **Ligne 50**: `case IN_LOCATION_BLOCK: // TODO`
- **Ligne 86**: `LocationsConfig ConfigParser::parseLocations(const std::vector<std::string> & lines) const{ //TODO : parse locations`

### ./config/ConfigValidator.cpp

- **Ligne 37**: `std::string ConfigValidator::getLastError() const{//todo oo correct this func`
- **Ligne 242**: `if (codeNum < 400 || codeNum > 599){ // Todo check error code min max, and if we're gonne use them all`
- **Ligne 302**: `setError("should we accept empty method"+ methods ); //todo maybe accept some empty methods`
- **Ligne 309**: `if (m != "GET" && m != "POST" && m != "DELETE" ){ // todo can add more if bonuses`
- **Ligne 320**: `setError("Invalid client max body size format: " +size); //todo ask bebou for MBS`
- **Ligne 326**: `if (sizeNum <= 0 || sizeNum > 1000000000){ //todo decrease bc 1 gb is kinda overkill see commetn above`
- **Ligne 355**: `bool ConfigValidator::validateIndex(const std::string& index){ //todo`
- **Ligne 361**: `bool ConfigValidator::isPortUsed(int port) const{ //todo do we need a check for free port?`

### ./server/onConnection.cpp

- **Ligne 30**: `//todo`

