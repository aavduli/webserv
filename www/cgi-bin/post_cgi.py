#!/usr/bin/env python3
import os
import sys

#headers
print("Content-Type: text/html")
print("")


content_length = int(os.environ.get('CONTENT_LENGTH', 0))

#read body from stdin
if content_length > 0:
	body_bytes= sys.stdin.buffer.read(content_length)
	try :
		body = body_bytes.decode('utf-8')
	except UnicodeDecodeError:
		body = body_bytes.decode('latin-1')
else:
	body = ""

#parse data (for application/x-www-form-urlencoded)
data = {}
if body:
	pairs= body.split('&')
	for pair in pairs:
		if '=' in pair:
			key, value = pair.split('=', 1)
			#decode url basiqe
			key = key.replace('+', ' ')
			value = value.replace('+', ' ')
			data[key] = value

#gen html resposnse
print("<html><body>")
print("<h1>CGI POST test</h1>")
print(f"<p>REQUEST_METHOD: {os.environ.get('REQUEST_METHOD', 'Unknown')}</p>")
print(f"<p>CONTENT_LENGTH: {content_length}</p>")
print(f"<p>CONTENT_TYPE: {os.environ.get('CONTENT_TYPE', 'Uknown')}</p>")
print(f"<p>raw body: {body}</p>")

#print pardsed data
for key, value in data.items():
	print(f"<p>{key} = {value}</p>")
print("<p><a href='/cgi-test/'>Back to CGI Tests</a></p>")
print("</body></html>")
