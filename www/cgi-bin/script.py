#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html\r")
print("\r")
print("<html><body>")
print("<h1>CGI Test Success</h1>")
print(f"<p>REQUEST_METHOD: {os.environ.get('REQUEST_METHOD')}</p>")
print(f"<p>CONTENT_LENGTH: {os.environ.get('CONTENT_LENGTH')}</p>")

if os.environ.get('REQUEST_METHOD') == 'POST':
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    post_data = sys.stdin.read(content_length)
    print(f"<p>POST Data: {post_data}</p>")

print("</body></html>")
