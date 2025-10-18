#!/bin/bash

# POST Request Testing Script
# Usage: ./test_post.sh [port]

PORT=${1:-8080}
BASE_URL="http://localhost:$PORT"

echo "=== POST Request Testing Script ==="
echo "Testing webserver at: $BASE_URL"
echo

# Test 1: Simple URL-encoded form
echo "Test 1: URL-encoded POST"
curl -X POST "$BASE_URL/success" \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "username=testuser&password=testpass&email=test@example.com" \
  -v
echo -e "\n---\n"

# Test 2: Text content
echo "Test 2: Text content POST"
curl -X POST "$BASE_URL/success" \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "message=This+is+a+test+message&category=feedback" \
  -v
echo -e "\n---\n"

# Test 3: File upload (multipart)
echo "Test 3: File upload (multipart)"
curl -X POST "$BASE_URL/upload" \
  -F "description=Test file upload" \
  -F "upload_file=@/home/angela/42/webserv/www/uploads/sample_upload.txt" \
  -F "notes=Testing multipart upload" \
  -v
echo -e "\n---\n"

# Test 4: Empty POST
echo "Test 4: Empty POST body"
curl -X POST "$BASE_URL/success" \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "" \
  -v
echo -e "\n---\n"

# Test 5: Large POST body (should trigger size limit)
echo "Test 5: Large POST body"
LARGE_DATA=$(printf 'data=%.0s' {1..2000})  # Create large data string
curl -X POST "$BASE_URL/success" \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "$LARGE_DATA" \
  -v
echo -e "\n---\n"

# Test 6: CGI POST
echo "Test 6: CGI POST processing"
curl -X POST "$BASE_URL/cgi_bin/post_test.py" \
  -H "Content-Type: application/x-www-form-urlencoded" \
  -d "username=cgi_user&message=Hello+from+CGI+test" \
  -v
echo -e "\n---\n"

# Test 7: Invalid Content-Type
echo "Test 7: Invalid Content-Type"
curl -X POST "$BASE_URL/success" \
  -H "Content-Type: invalid/type" \
  -d "test=data" \
  -v
echo -e "\n---\n"

# Test 8: Missing Content-Type
echo "Test 8: Missing Content-Type"
curl -X POST "$BASE_URL/success" \
  -d "test=data" \
  -v
echo -e "\n---\n"

echo "=== Testing Complete ==="