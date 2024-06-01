#!/usr/bin/env python3

import cgi
import cgitb
import os

# Activer le mode debug pour les erreurs CGI
cgitb.enable()

print("Content-Type: text/html\r")    # HTML est le type de contenu
print("\r")                             # Ligne vide pour terminer les en-têtes

# Récupérer les données du formulaire
form = cgi.FieldStorage()

# Code HTML pour afficher le message et les variables d'environnement
print("""<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Bonjour CGI</title>
</head>
<body>
    <h1>Bonjour, le monde!</h1>
    <h2>Variables d'environnement</h2>
    <table border="1">
        <tr>
            <th>Variable</th>
            <th>Valeur</th>
        </tr>
""")

# Afficher toutes les variables d'environnement
for key in os.environ.keys():
    print(f"<tr><td>{key}</td><td>{os.environ[key]}</td></tr>")

print("""
    </table>
    <h2>Corps de la requête</h2>
    <pre>
""")

# Afficher le corps de la requête
if form:
    for field in form.keys():
        print(f"{field}: {form[field].value}")
else:
    print("Pas de données POST reçues")

print("""
    </pre>
</body>
</html>
""")
