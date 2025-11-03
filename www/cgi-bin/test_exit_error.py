#!/usr/bin/env python3

print("Content-Type: text/html\r")
print("\r")
print("<html><body>")
print("<h1>Testing Exit with Error Code</h1>")

import sys
exit(42)

print("</body></html>")
