#!/bin/bash
# Error Handling & Edge Cases Tests for webserv
# Usage: chmod +x test_errors.sh && ./test_errors.sh

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

print_test_header "ERROR HANDLING & EDGE CASES"

# Test 1: 404 Not Found
print_test "1" "GET /does-not-exist" "404 Not Found - custom error page"
curl -i $SERVER/does-not-exist
print_separator

# Test 2: 405 Method Not Allowed
print_test "2" "DELETE /browse (method not allowed)" "405 Method Not Allowed - custom error page"
curl -i -X DELETE $SERVER/browse/
print_separator

# Test 3: 413 Payload Too Large
print_test "3" "POST with large body" "413 Payload Too Large"
curl -i -X POST $SERVER/test-post \
  -H "Content-Type: text/plain" \
  -d "$(head -c 2048 /dev/zero | tr '\0' 'A')"
print_separator

# Test 4: 400 Bad Request (malformed request)
print_test "4" "Malformed HTTP request" "400 Bad Request"
echo -e "GET /test HTTP/1.1\r\nBad Header\r\n\r\n" | timeout 3 nc localhost 8080
print_separator

# Test 5: 411 Length Required
print_test "5" "POST without Content-Length" "411 Length Required"
echo -e "POST /test-post HTTP/1.1\r\nHost: localhost\r\n\r\n" | timeout 3 nc localhost 8080
print_separator

# Test 6: 415 Unsupported Media Type
print_test "6" "POST with unsupported content type" "415 Unsupported Media Type"
curl -i -X POST $SERVER/test-post \
  -H "Content-Type: application/xml" \
  -d "<test>data</test>"
print_separator

# Test 7: HTTP/1.0 request
print_test "7" "HTTP/1.0 request" "200 OK - server handles HTTP/1.0"
echo -e "GET / HTTP/1.0\r\nHost: localhost\r\n\r\n" | timeout 3 nc localhost 8080
print_separator

# Test 8: Request with no Host header (HTTP/1.1)
print_test "8" "HTTP/1.1 without Host header" "400 Bad Request"
echo -e "GET / HTTP/1.1\r\n\r\n" | timeout 3 nc localhost 8080
print_separator

# Test 9: Very long URL
print_test "9" "Very long URL (>2048 chars)" "414 URI Too Long"
LONG_PATH=$(printf "a%.0s" {1..2100})
curl -i "$SERVER/$LONG_PATH"
print_separator

# Test 10: Request with many headers
print_test "10" "Request with many headers" "200 OK"
curl -i $SERVER/ \
  -H "Custom-Header-1: value1" \
  -H "Custom-Header-2: value2" \
  -H "Custom-Header-3: value3" \
  -H "Custom-Header-4: value4" \
  -H "Custom-Header-5: value5"
print_separator

# Test 11: Invalid HTTP method
print_test "11" "Invalid HTTP method" "405 Method Not Allowed"
echo -e "INVALID / HTTP/1.1\r\nHost: localhost\r\n\r\n" | timeout 3 nc localhost 8080
print_separator

# Test 12: Null bytes in URL
print_test "12" "URL with null bytes" "400 Bad Request"
curl -i "$SERVER/test%00.txt"
print_separator

# Test 13: Connection timeout test
print_test "13" "Connection timeout" "Server should close idle connections"
echo -e "${YELLOW}Opening connection and waiting (30s timeout)...${NC}"
(sleep 35; echo -e "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n") | timeout 3 nc localhost 8080 &
sleep 5
echo -e "${CYAN}Connection should timeout and close${NC}"
print_separator

# Test 14: Multiple rapid requests (stress test)
print_test "14" "10 concurrent requests" "All should return 200 OK"
echo -e "${YELLOW}Sending 10 concurrent requests...${NC}"
for i in {1..10}; do
  curl -s -w "Request $i: %{http_code}\n" $SERVER/ > /dev/null &
done
wait
print_separator

# Test 15: OPTIONS method (if supported)
print_test "15" "OPTIONS method" "405 Method Not Allowed"
curl -i -X OPTIONS $SERVER/files
print_separator

print_test_header "REDIRECT TESTS"

# Test 16: 301 Permanent redirect
print_test "16" "GET /redirect" "301 Moved Permanently"
curl -i $SERVER/redirect
print_separator

# Test 17: 302 Temporary redirect
print_test "17" "GET /redirect-tmp" "302 Found"
curl -i $SERVER/redirect-tmp
print_separator

echo -e "\n${GREEN}✅ Error handling & edge case tests completed!${NC}"
echo -e "${YELLOW}Review results above for proper error handling${NC}"