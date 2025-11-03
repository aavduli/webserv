#!/bin/bash

echo "🧪 Testing Multi-Server Configuration"
echo "======================================"
echo ""

# Couleurs
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Compteurs
PASSED=0
FAILED=0

test_endpoint() {
    local method=$1
    local url=$2
    local expected_code=$3
    local description=$4
    local data=$5

    echo -n "  $description... "

    if [ -z "$data" ]; then
        response=$(curl -s -o /dev/null -w "%{http_code}" -X $method "$url" 2>/dev/null)
    else
        response=$(curl -s -o /dev/null -w "%{http_code}" -X $method "$url" -d "$data" 2>/dev/null)
    fi

    if [ "$response" == "$expected_code" ]; then
        echo -e "${GREEN}✓${NC} (HTTP $response)"
        ((PASSED++))
    else
        echo -e "${RED}✗${NC} (Expected $expected_code, got $response)"
        ((FAILED++))
    fi
}

test_content() {
    local method=$1
    local url=$2
    local expected_pattern=$3
    local description=$4

    echo -n "  $description... "

    response=$(curl -s -X $method "$url" 2>/dev/null)

    if echo "$response" | grep -q "$expected_pattern"; then
        echo -e "${GREEN}✓${NC} (Found '$expected_pattern')"
        ((PASSED++))
    else
        echo -e "${RED}✗${NC} (Pattern '$expected_pattern' not found)"
        ((FAILED++))
    fi
}

# Vérifier que le serveur tourne
echo "🔍 Checking server availability..."
if ! curl -s http://localhost:8080/ > /dev/null 2>&1; then
    echo -e "${RED}✗ Server not running! Start it with: ./webserv configfiles/multi.conf${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Server is running${NC}"
echo ""

# ========================================
# SERVER 1 (8080) - HTML Site
# ========================================
echo -e "${BLUE}🟦 Server 1 (8080) - HTML Site${NC}"
echo "   Root: ./www/html"
echo "   Locations: / (GET POST), /cgi-bin (GET POST)"
echo ""

# Location / tests
echo "  Location /"
test_endpoint GET "http://localhost:8080/" 200 "GET / (allowed)"
test_endpoint POST "http://localhost:8080/" 200 "POST / (allowed)" "test=data"
test_endpoint DELETE "http://localhost:8080/" 405 "DELETE / (forbidden)"
test_endpoint PUT "http://localhost:8080/" 405 "PUT / (not configured)"

# Location /cgi-bin tests
echo ""
echo "  Location /cgi-bin"
test_endpoint GET "http://localhost:8080/cgi-bin/" 200 "GET /cgi-bin (allowed)"
test_endpoint POST "http://localhost:8080/cgi-bin/" 200 "POST /cgi-bin (allowed)"
test_endpoint DELETE "http://localhost:8080/cgi-bin/" 405 "DELETE /cgi-bin (forbidden)"

# Error page test
echo ""
echo "  Error handling"
test_endpoint GET "http://localhost:8080/nonexistent" 404 "GET /nonexistent (404 custom page)"
test_content GET "http://localhost:8080/nonexistent" "404" "Custom 404 page content"

# Body size limit test
echo ""
echo "  Body size limits (max: 1024 bytes)"
test_endpoint POST "http://localhost:8080/" 200 "POST with 500 bytes (ok)" "$(printf 'a%.0s' {1..500})"
test_endpoint POST "http://localhost:8080/" 413 "POST with 2000 bytes (too large)" "$(printf 'a%.0s' {1..2000})"

echo ""

# ========================================
# SERVER 2 (9090) - File Manager
# ========================================
echo -e "${BLUE}🟩 Server 2 (9090) - File Manager${NC}"
echo "   Root: ./www/file_manager"
echo "   Locations: / (GET), /files (GET POST DELETE)"
echo ""

# Location / tests
echo "  Location /"
test_endpoint GET "http://localhost:9090/" 200 "GET / (allowed)"
test_endpoint POST "http://localhost:9090/" 405 "POST / (forbidden - only GET)"
test_endpoint DELETE "http://localhost:9090/" 405 "DELETE / (forbidden)"

# Location /files tests
echo ""
echo "  Location /files"
test_endpoint GET "http://localhost:9090/files/" 200 "GET /files (allowed)"
test_endpoint POST "http://localhost:9090/files/" 200 "POST /files (upload allowed)"
test_endpoint DELETE "http://localhost:9090/files/test.txt" 404 "DELETE /files/test.txt (file not found)"

# Autoindex test
echo ""
echo "  Autoindex (enabled on /)"
test_content GET "http://localhost:9090/" "Index of" "Directory listing displayed"

# Body size limit test
echo ""
echo "  Body size limits (max: 2048 bytes)"
test_endpoint POST "http://localhost:9090/files/" 200 "POST with 1000 bytes (ok)" "$(printf 'a%.0s' {1..1000})"
test_endpoint POST "http://localhost:9090/files/" 413 "POST with 3000 bytes (too large)" "$(printf 'a%.0s' {1..3000})"

echo ""

# ========================================
# SERVER 3 (8888) - POST Form
# ========================================
echo -e "${BLUE}🟨 Server 3 (8888) - POST Form${NC}"
echo "   Root: ./www/post"
echo "   Locations: / (GET POST), /cgi-bin (DELETE only!)"
echo ""

# Location / tests
echo "  Location /"
test_endpoint GET "http://localhost:8888/" 200 "GET / (allowed)"
test_endpoint POST "http://localhost:8888/" 200 "POST / (allowed)" "username=test&password=secret"
test_endpoint DELETE "http://localhost:8888/" 405 "DELETE / (forbidden)"

# Location /cgi-bin tests (special: DELETE only!)
echo ""
echo "  Location /cgi-bin (⚠️ DELETE ONLY!)"
test_endpoint GET "http://localhost:8888/cgi-bin/" 405 "GET /cgi-bin (forbidden - only DELETE!)"
test_endpoint POST "http://localhost:8888/cgi-bin/" 405 "POST /cgi-bin (forbidden - only DELETE!)"
test_endpoint DELETE "http://localhost:8888/cgi-bin/test.py" 404 "DELETE /cgi-bin/test.py (file not found)"

# No custom error page
echo ""
echo "  Error handling (no custom error page)"
test_content GET "http://localhost:8888/nonexistent" "404" "Default 404 error page"

echo ""

# ========================================
# ISOLATION TESTS (Critical!)
# ========================================
echo -e "${YELLOW}🔒 Testing Server Isolation${NC}"
echo "   Verify each server uses its own config"
echo ""

echo "  CGI /cgi-bin isolation"
test_endpoint GET "http://localhost:8080/cgi-bin/" 200 "Port 8080: GET /cgi-bin ALLOWED"
test_endpoint GET "http://localhost:8888/cgi-bin/" 405 "Port 8888: GET /cgi-bin FORBIDDEN (only DELETE)"

echo ""
echo "  POST method isolation on /"
test_endpoint POST "http://localhost:8080/" 200 "Port 8080: POST / ALLOWED" "test=data"
test_endpoint POST "http://localhost:9090/" 405 "Port 9090: POST / FORBIDDEN (only GET)"
test_endpoint POST "http://localhost:8888/" 200 "Port 8888: POST / ALLOWED" "test=data"

echo ""
echo "  DELETE method isolation"
test_endpoint DELETE "http://localhost:8080/" 405 "Port 8080: DELETE / FORBIDDEN"
test_endpoint DELETE "http://localhost:9090/files/x" 404 "Port 9090: DELETE /files ALLOWED (404 = method ok)"
test_endpoint DELETE "http://localhost:8888/" 405 "Port 8888: DELETE / FORBIDDEN"

echo ""
echo "  Root directory isolation"
test_content GET "http://localhost:8080/" "html" "Port 8080 uses ./www/html"
test_content GET "http://localhost:9090/" "file_manager" "Port 9090 uses ./www/file_manager"
test_content GET "http://localhost:8888/" "form" "Port 8888 uses ./www/post"

echo ""
echo "  Client max body size isolation"
test_endpoint POST "http://localhost:8080/" 413 "Port 8080: 1500 bytes REJECTED (max 1024)" "$(printf 'a%.0s' {1..1500})"
test_endpoint POST "http://localhost:9090/files/" 200 "Port 9090: 1500 bytes ACCEPTED (max 2048)" "$(printf 'a%.0s' {1..1500})"

echo ""

# ========================================
# SUMMARY
# ========================================
echo "======================================"
echo -e "${BLUE}📊 Test Summary${NC}"
echo "======================================"
echo -e "Total tests:  $((PASSED + FAILED))"
echo -e "${GREEN}Passed:       $PASSED${NC}"
echo -e "${RED}Failed:       $FAILED${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✅ All tests passed! Multi-server configuration is working correctly.${NC}"
    exit 0
else
    echo -e "${RED}❌ Some tests failed. Check the output above for details.${NC}"
    exit 1
fi
