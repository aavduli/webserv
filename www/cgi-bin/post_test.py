#!/usr/bin/env python3

import os
import sys
import cgi
import cgitb

# Enable CGI error reporting
cgitb.enable()

# Print HTTP headers
print("Content-Type: text/html")
print()  # Empty line required between headers and body

# HTML response
print("""<!DOCTYPE html>
<html>
<head>
    <title>CGI POST Test Result</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        .result { background: #f0f8ff; padding: 20px; border-radius: 5px; }
        .env-var { background: #f9f9f9; padding: 5px; margin: 2px 0; }
        pre { background: #f5f5f5; padding: 10px; overflow: auto; }
    </style>
</head>
<body>
    <h1>CGI POST Processing Result</h1>
    <div class="result">""")

# Display request method
print(f"<h2>Request Method: {os.environ.get('REQUEST_METHOD', 'Unknown')}</h2>")

# Display content type and length
content_type = os.environ.get('CONTENT_TYPE', 'Not specified')
content_length = os.environ.get('CONTENT_LENGTH', '0')
print(f"<p><strong>Content-Type:</strong> {content_type}</p>")
print(f"<p><strong>Content-Length:</strong> {content_length}</p>")

# Process POST data
if os.environ.get('REQUEST_METHOD') == 'POST':
    try:
        # Read POST data from stdin
        post_data = sys.stdin.read(int(content_length)) if content_length.isdigit() else ""
        
        print("<h3>Raw POST Data:</h3>")
        print(f"<pre>{post_data}</pre>")
        
        # Try to parse form data
        if 'application/x-www-form-urlencoded' in content_type:
            print("<h3>Parsed Form Data (URL-encoded):</h3>")
            # Simple parsing for demonstration
            pairs = post_data.split('&')
            for pair in pairs:
                if '=' in pair:
                    key, value = pair.split('=', 1)
                    # Basic URL decoding
                    key = key.replace('+', ' ')
                    value = value.replace('+', ' ')
                    print(f"<div class='env-var'><strong>{key}:</strong> {value}</div>")
        
        elif 'multipart/form-data' in content_type:
            print("<h3>Multipart Form Data Detected</h3>")
            print("<p>This CGI script detected multipart form data. ")
            print("Full parsing would require additional libraries.</p>")
            
    except Exception as e:
        print(f"<p><strong>Error processing POST data:</strong> {e}</p>")

# Display relevant environment variables
print("<h3>Relevant Environment Variables:</h3>")
env_vars = ['REQUEST_METHOD', 'CONTENT_TYPE', 'CONTENT_LENGTH', 'QUERY_STRING', 
           'HTTP_HOST', 'SERVER_NAME', 'SERVER_PORT', 'PATH_INFO']

for var in env_vars:
    value = os.environ.get(var, 'Not set')
    print(f"<div class='env-var'><strong>{var}:</strong> {value}</div>")

print("""    </div>
	<p><a href='/cgi-test/'>Back to CGI Tests</a></p>
</body>
</html>""")