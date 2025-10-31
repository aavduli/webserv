#!/usr/bin/env python3
import time
import sys
import os

# Required CGI headers
print("Content-Type: text/html")
print("")

# Get timeout parameter from query string
query_string = os.environ.get('QUERY_STRING', '')
seconds = 2  # default

if query_string:
    for param in query_string.split('&'):
        if param.startswith('seconds='):
            try:
                seconds = int(param.split('=')[1])
                seconds = max(1, min(10, seconds))  # Clamp between 1-10
            except:
                seconds = 2

print("<html>")
print("<head><title>CGI Timeout Test</title></head>")
print("<body>")
print(f"<h1>Timeout Test - Waiting {seconds} seconds...</h1>")
print("<p>This script will wait and then show a result.</p>")

# Flush output
sys.stdout.flush()

# Wait the specified time
time.sleep(seconds)

print(f"<p><strong>Success!</strong> Waited {seconds} seconds.</p>")
print("<p><a href='/cgi-test/'>Back to CGI Tests</a></p>")
print("</body>")
print("</html>")
