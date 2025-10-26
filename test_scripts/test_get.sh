#!/bin/bash
# GET Method Tests for webserv
# Usage: chmod +x test_get.sh && ./test_get.sh

# Color definitions
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
WHITE='\033[1;37m'
NC='\033[0m' # No Color

SERVER="http://localhost:8080"

print_test_header() {
    echo -e "\n${CYAN}===========================================${NC}"
    echo -e "${WHITE}$1${NC}"
    echo -e "${CYAN}===========================================${NC}"
}

print_test() {
    local test_num="$1"
    local description="$2"
    local expected="$3"
    
    echo -e "\n${YELLOW}[TEST $test_num]${NC} ${WHITE}$description${NC}"
    echo -e "${BLUE}Expected:${NC} $expected"
    echo -e "${PURPLE}Result:${NC}"
}

print_separator() {
    echo -e "\n${CYAN}-------------------------------------------${NC}"
}

print_test_header "GET METHOD TESTS"

# Test 1: GET root (index.html)
print_test "1" "GET / (root)" "200 OK - index.html content"
curl -i $SERVER/
print_separator

# Test 2: GET specific HTML file
print_test "2" "GET /index.html" "200 OK - index.html content"
curl -i $SERVER/index.html
print_separator

# Test 3: GET with autoindex enabled
print_test "3" "GET /browse (directory listing)" "200 OK - directory listing HTML"
curl -i $SERVER/browse/
print_separator

# Test 4: GET form page
print_test "4" "GET /form" "200 OK - form.html content"
curl -i $SERVER/form
print_separator

# Test 5: GET file manager page
print_test "5" "GET /file_manager" "200 OK - file_manager.html content"
curl -i $SERVER/file_manager
print_separator

# Test 6: GET success page
print_test "6" "GET /success" "200 OK - success.html content"
curl -i $SERVER/success
print_separator

# Test 7: GET with autoindex on /files
print_test "7" "GET /files (directory listing)" "200 OK - files directory listing"
curl -i $SERVER/files/
print_separator

# Test 8: GET non-existent file (404)
print_test "8" "GET /nonexistent.html" "404 Not Found - custom error page"
curl -i $SERVER/nonexistent.html
print_separator

# Test 9: GET method not allowed (405)
print_test "9" "GET /test-post" "405 Method Not Allowed (only POST allowed)"
curl -i $SERVER/test-post
print_separator

# Test 10: GET with query parameters
print_test "10" "GET with query params" "200 OK - params should be accessible to server"
curl -i "$SERVER/?param1=value1&param2=value2"
print_separator

# Test 11: GET CGI script
print_test "11" "GET /cgi-bin/script.py" "200 OK - CGI script execution"
curl -i "$SERVER/cgi-bin/script.py?name=test&message=hello"
print_separator

# Test 12: HEAD request
print_test "12" "HEAD /" "405 Method Not Allowed - headers only, no body"
curl -I $SERVER/
print_separator

echo -e "\n${GREEN}✅ GET method tests completed!${NC}"
echo -e "${YELLOW}Check results above for any unexpected responses${NC}"