#!/usr/bin/env python

import os
import cgi
import cgitb
import sys

cgitb.enable()  # Pour le débogage en cas d'erreur

# Définir le répertoire de téléchargement
UPLOAD_DIR = os.environ.get('TMPDIR', '/tmp')

# Vérifier si le répertoire de téléchargement existe
if not os.path.exists(UPLOAD_DIR):
    try:
        # Créer le répertoire
        os.makedirs(UPLOAD_DIR)
        # Définir les permissions (lecture, écriture et exécution pour le propriétaire, le groupe et les autres)
        os.chmod(UPLOAD_DIR, 0o777)
    except Exception as e:
        print("Status: 500 Internal Server Error\r\n")
        print("Content-Type: text/html\r\n")
        print("\r\n")
        print(f"<html><body><h1>Internal Server Error</h1><p>{str(e)}</p></body></html>")
        sys.exit(1)

def main():
    try:
        # Imprimer les en-têtes de la requête pour le débogage
        print("Content-Type: text/html\r\n")
        print("<html><body><pre>")
        for key, value in os.environ.items():
            print(f"{key}: {value}")
        print("</pre></body></html>")
        
        # Lire les en-têtes et le corps de la requête
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
        if content_length == 0:
            print("Status: 400 Bad Request\r\n")
            print("Content-Type: text/html\r\n")
            print("\r\n")
            print("<html><body><h1>Bad Request: No content received</h1></body></html>")
            return

        form = cgi.FieldStorage(fp=sys.stdin, environ=os.environ, keep_blank_values=True)

        # Vérifier si le champ 'file' est dans le formulaire
        if 'file' not in form:
            print("Status: 400 Bad Request\r\n")
            print("Content-Type: text/html\r\n")
            print("\r\n")
            print("<html><body><h1>Bad Request: No file field in form</h1></body></html>")
            return

        fileitem = form['file']

        # Vérifier si un fichier a été téléchargé
        if fileitem.filename:
            # Nettoyer le nom du fichier pour éviter les problèmes de sécurité
            filename = os.path.basename(fileitem.filename)
            filepath = os.path.join(UPLOAD_DIR, filename)

            # Écrire le fichier sur le disque
            with open(filepath, 'wb') as f:
                f.write(fileitem.file.read())

            # Répondre au client
            print("Status: 201 Created\r\n")
            print("Content-Type: text/html\r\n")
            print("\r\n")
            print(f"<html><body><h1>File '{filename}' uploaded successfully</h1></body></html>")
        else:
            print("Status: 400 Bad Request\r\n")
            print("Content-Type: text/html\r\n")
            print("\r\n")
            print("<html><body><h1>Bad Request: No file uploaded</h1></body></html>")
    except Exception as e:
        print("Status: 500 Internal Server Error\r\n")
        print("Content-Type: text/html\r\n")
        print("\r\n")
        print(f"<html><body><h1>Internal Server Error</h1><p>{str(e)}</p></body></html>")

if __name__ == "__main__":
    main()
