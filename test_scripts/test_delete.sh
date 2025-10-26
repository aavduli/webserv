#!/bin/bash
# DELETE Method Tests for webserv
# Usage: chmod +x test_delete.sh && ./test_delete.sh

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

print_test_header "DELETE METHOD TESTS"

# Create test files for deletion tests
echo "Delete test file 1" > /tmp/delete_test1.txt
echo "Delete test file 2" > /tmp/delete_test2.txt
curl -s -X POST $SERVER/files -F "file1=@/tmp/delete_test1.txt" > /dev/null
curl -s -X POST $SERVER/files -F "file1=@/tmp/delete_test2.txt" > /dev/null
echo -e "${GREEN}✅ Test files uploaded${NC}"
# List files first to see actual filename
echo -e "${CYAN}Files currently in files:${NC}"
curl -s $SERVER/browse | grep -o 'delete_test1[^"]*\.txt' | head -1

# Test 1: DELETE uploaded file (direct DELETE request)
print_test "1" "DELETE /files/delete_test1*.txt" "200 OK or 204 No Content - file deleted"
FILENAME=$(curl -s $SERVER/browse | grep -o 'delete_test1[^"]*\.txt' | head -1)
if [ ! -z "$FILENAME" ]; then
    curl -i -X DELETE $SERVER/files/$FILENAME
else
    echo -e "${RED}❌ No delete_test1 file found to delete${NC}"
fi
print_separator

# Test 2: DELETE via POST with method override
print_test "2" "POST /files with _method=DELETE" "200 OK - file deleted via form"
FILENAME2=$(curl -s $SERVER/browse | grep -o 'delete_test2[^"]*\.txt' | head -1)
if [ ! -z "$FILENAME2" ]; then
    curl -i -X POST $SERVER/files \
      -H "Content-Type: application/x-www-form-urlencoded" \
      -d "_method=DELETE&filename=$FILENAME2"
else
    echo -e "${RED}❌ No delete_test2 file found to delete${NC}"
fi
print_separator

# Test 3: DELETE non-existent file (404)
print_test "3" "DELETE non-existent file" "404 Not Found"
curl -i -X DELETE $SERVER/files/nonexistent_file.txt
print_separator

# Test 4: DELETE method not allowed (405)
print_test "4" "DELETE /form (method not allowed)" "405 Method Not Allowed"
curl -i -X DELETE $SERVER/form
print_separator

# Test 5: DELETE on root (405)
print_test "5" "DELETE / (method not allowed)" "405 Method Not Allowed"
curl -i -X DELETE $SERVER/
print_separator

# Test 6: DELETE with form but missing filename
print_test "6" "POST /files with _method=DELETE but no filename" "400 Bad Request"
curl -i -X POST $SERVER/files \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "_method=DELETE&filename="
print_separator

# Test 7: DELETE with invalid characters in filename
print_test "7" "DELETE with filename escaping root" "403 Forbidden"
curl -i -X POST $SERVER/files \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "_method=DELETE&filename=../../../etc/passwd"
print_separator

echo -e "\n${GREEN}✅ DELETE method tests completed!${NC}"
echo -e "${YELLOW}Check ./www/file_manager/files/ to verify deletions${NC}"

# Cleanup
echo -e "\n${CYAN}Cleanup temporary files? (y/n)${NC}"
read -r cleanup
if [[ $cleanup == "y" || $cleanup == "Y" ]]; then
    rm -f /tmp/delete_test1.txt /tmp/delete_test2.txt
    echo -e "${GREEN}✅ Temporary files cleaned up${NC}"
fi
fi