#!/usr/bin/env python3
import os
import cgi
import cgitb

cgitb.enable()  # Activer le débogage

UPLOAD_DIR = '/tmp'  # Répertoire où les fichiers seront stockés
FILE_NAME = 'testfile.txt'
FILE_PATH = os.path.join(UPLOAD_DIR, FILE_NAME)

def handle_post():
    form = cgi.FieldStorage()
    if 'content' in form:
        content = form.getvalue('content')
        with open(FILE_PATH, 'a') as f:
            f.write(content + '\n')
        print("Status: 201 Created\r")
        print("Content-Type: text/html\r")
        print("\r")
        print("<html><body><h1>Content added successfully</h1></body></html>")
    else:
        print("Status: 400 Bad Request\r")
        print("Content-Type: text/html\r")
        print("\r")
        print("<html><body><h1>Bad Request: No content provided</h1></body></html>")

def handle_delete():
    form = cgi.FieldStorage()
    if 'content' in form:
        content = form.getvalue('content')
        if os.path.exists(FILE_PATH):
            with open(FILE_PATH, 'r') as f:
                lines = f.readlines()
            with open(FILE_PATH, 'w') as f:
                for line in lines:
                    if line.strip() != content:
                        f.write(line)
            print("Status: 200 OK\r")
            print("Content-Type: text/html\r")
            print("\r")
            print("<html><body><h1>Content deleted successfully</h1></body></html>")
        else:
            print("Status: 404 Not Found\r")
            print("Content-Type: text/html\r")
            print("\r")
            print("<html><body><h1>Not Found: File does not exist</h1></body></html>")
    else:
        print("Status: 400 Bad Request\r")
        print("Content-Type: text/html\r")
        print("\r")
        print("<html><body><h1>Bad Request: No content provided</h1></body></html>")

def main():
    method = os.environ.get('REQUEST_METHOD', '')
    if method == 'POST':
        handle_post()
    elif method == 'DELETE':
        handle_delete()
    else:
        print("Status: 405 Method Not Allowed\r")
        print("Content-Type: text/html\r")
        print("\r")
        print("<html><body><h1>Method Not Allowed</h1></body></html>")

if __name__ == "__main__":
    main()
