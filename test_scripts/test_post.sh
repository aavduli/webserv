#!/bin/bash
# POST Method Tests for webserv
# Usage: chmod +x test_post.sh && ./test_post.sh

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

print_test_header "POST METHOD TESTS"

# Test 1: POST form data (urlencoded)
print_test "1" "POST /test-post with form data" "200 OK - form data processed"
curl -i -X POST $SERVER/test-post \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "username=testuser&email=test@example.com&message=Hello+World"
print_separator

# Test 2: POST to form endpoint
print_test "2" "POST /form with form data" "200 OK - form submission success"
curl -i -X POST $SERVER/form \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "name=John&age=30&country=USA"
print_separator

# Test 3: POST file upload (single file)
print_test "3" "POST /files - Upload single file" "200 OK - file uploaded successfully"
echo "This is a test file for upload" > /tmp/test_upload.txt
curl -i -X POST $SERVER/files \
  -F "file1=@/tmp/test_upload.txt"
print_separator

# Test 4: POST multiple files
print_test "4" "POST /files - Upload multiple files" "200 OK - multiple files uploaded"
echo "File 1 content" > /tmp/test1.txt
echo "File 2 content" > /tmp/test2.txt
curl -i -X POST $SERVER/files \
  -F "file1=@/tmp/test1.txt" \
  -F "file2=@/tmp/test2.txt"
print_separator

# Test 5: POST to CGI endpoint
print_test "5" "POST /cgi-bin/post_test.py" "200 OK - CGI script processes POST data"
curl -i -X POST $SERVER/cgi-bin/post_test.py \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "param1=value1&param2=value2&name=PostTest"
print_separator

# Test 6: POST to CGI test endpoint
print_test "6" "POST /cgi-test/post_cgi.py" "200 OK - CGI POST processing"
curl -i -X POST $SERVER/cgi-test/post_cgi.py \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "test=cgi&data=example"
print_separator

# Test 7: POST JSON data
print_test "7" "POST /test-post with JSON" "415 Unsupported Media Type"
curl -i -X POST $SERVER/test-post \
  -H "Content-Type: application/json" \
  -d '{"username":"jsonuser","email":"json@test.com"}'
print_separator

# Test 8: POST exceeding body size limit (413)
print_test "8" "POST with large body" "413 Payload Too Large"
curl -i -X POST $SERVER/test-post \
  -H "Content-Type: text/plain" \
  -d "$(head -c 2048 /dev/zero | tr '\0' 'A')"
print_separator

# Test 9: POST method not allowed (405)
print_test "9" "POST /browse (method not allowed)" "405 Method Not Allowed"
curl -i -X POST $SERVER/browse/ \
  -d "data=test"
print_separator

# Test 10: POST without Content-Length (411)
print_test "10" "POST without Content-Length" "411 Length Required"
echo -e "POST /test-post HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n" | timeout 3 nc localhost 8080
print_separator

# Test 11: POST empty body
print_test "11" "POST /test-post with empty body" "400 Bad Request - empty body not accepted"
curl -i -X POST $SERVER/test-post \
  -H "Content-Length: 0"
print_separator

# Test 12: POST with method override for DELETE
print_test "12" "POST with _method=DELETE override" "200 OK - file deleted via method override"
curl -i -X POST $SERVER/files \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "_method=DELETE&filename=test_upload.txt"
print_separator

echo -e "\n${GREEN}✅ POST method tests completed!${NC}"
echo -e "${YELLOW}Check ./www/file_manager/files for uploaded files${NC}"
echo -e "${RED}⚠️  Some test files created: /tmp/test*.txt${NC}"

# Cleanup option
echo -e "\n${CYAN}Cleanup temporary files? (y/n)${NC}"
read -r cleanup
if [[ $cleanup == "y" || $cleanup == "Y" ]]; then
    rm -f /tmp/test_upload.txt /tmp/test1.txt /tmp/test2.txt
    echo -e "${GREEN}✅ Temporary files cleaned up${NC}"
fi

