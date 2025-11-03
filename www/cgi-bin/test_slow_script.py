#!/usr/bin/env python3
import time

print("Content-Type: text/html\r")
print("\r")
print("<html><body>")
print("<h1>Testing Slow Script (70 seconds - more than TIMEOUT_CGI)</h1>")

time.sleep(70)

print("<p>This should never be sent</p>")
print("</body></html>")
