#!/usr/bin/env python3

print("Content-Type: text/html\r")
print("\r")

import this_module_does_not_exist

print("<html><body>")
print("<h1>This should never print</h1>")
print("</body></html>")
