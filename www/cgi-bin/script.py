#!/usr/bin/env python3

import os
import sys
from urllib.parse import parse_qs

print("Content-Type: text/html")
print("")

print("<html>")
print("<head><title>CGI Environment Test</title></head>")
print("<body>")

query_string = os.environ.get('QUERY_STRING', '')
if query_string:
	# Parser les paramètres
	params = parse_qs(query_string)

	# Extraire name et message
	name = params.get('name', [''])[0]
	message = params.get('message', [''])[0]

	print("<div class='result'>")
	print(f"<h2>Form Data Received:</h2>")
	print(f"<p><strong>Name:</strong> {name if name else 'Not provided'}</p>")
	print(f"<p><strong>Message:</strong> {message if message else 'Not provided'}</p>")
	print("</div>")
else:
	print("<p>No data received. Please fill the form.</p>")


print("<h1>CGI Environment Variables</h1>")

print("<h2>Basic CGI Variables:</h2>")
print(f"<p><strong>REQUEST_METHOD:</strong> {os.environ.get('REQUEST_METHOD', 'Not set')}</p>")
print(f"<p><strong>QUERY_STRING:</strong> {os.environ.get('QUERY_STRING', 'Not set')}</p>")
print(f"<p><strong>CONTENT_LENGTH:</strong> {os.environ.get('CONTENT_LENGTH', 'Not set')}</p>")
print(f"<p><strong>CONTENT_TYPE:</strong> {os.environ.get('CONTENT_TYPE', 'Not set')}</p>")

print("<h2>Server Variables:</h2>")
print(f"<p><strong>SERVER_PROTOCOL:</strong> {os.environ.get('SERVER_PROTOCOL', 'Not set')}</p>")
print(f"<p><strong>GATEWAY_INTERFACE:</strong> {os.environ.get('GATEWAY_INTERFACE', 'Not set')}</p>")
print(f"<p><strong>SERVER_NAME:</strong> {os.environ.get('SERVER_NAME', 'Not set')}</p>")
print(f"<p><strong>SERVER_PORT:</strong> {os.environ.get('SERVER_PORT', 'Not set')}</p>")
print(f"<p><strong>SCRIPT_NAME:</strong> {os.environ.get('SCRIPT_NAME', 'Not set')}</p>")
print(f"<p><strong>PATH_INFO:</strong> {os.environ.get('PATH_INFO', 'Not set')}</p>")
print(f"<p><strong>PATH_TRANSLATED:</strong> {os.environ.get('PATH_TRANSLATED', 'Not set')}</p>")
print(f"<p><strong>REMOTE_ADDR:</strong> {os.environ.get('REMOTE_ADDR', 'Not set')}</p>")

print("<h2>POST Data (from STDIN):</h2>")
if os.environ.get('REQUEST_METHOD') == 'POST':
	content_length = os.environ.get('CONTENT_LENGTH')
	if content_length:
		post_data = sys.stdin.read(int(content_length))
		print(f"<p><strong>POST Body:</strong> {post_data}</p>")
	else:
		print("<p>No CONTENT_LENGTH set</p>")
else:
	print("<p>Not a POST request</p>")



print("<h2>HTTP Headers:</h2>")
for key, value in os.environ.items():
	if key.startswith('HTTP_'):
		print(f"<p><strong>{key}:</strong> {value}</p>")

print("<h2>Working Directory:</h2>")
print(f"<p><strong>Current Dir:</strong> {os.getcwd()}</p>")



print("""    </div>
    <p><a href="/form">← Back to Test Forms</a></p>
</body>
</html>""")
