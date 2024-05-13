#!/bin/bash

# En-têtes HTTP
echo >> file1 "Test"
echo "Content-type: text/html\r\n"
echo ""

# Corps de la page HTML
echo "<!DOCTYPE html>"
echo "<html lang='en'>"
echo "<head>"
echo "<meta charset='UTF-8'>"
echo "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
echo "<title>Page HTML Simple</title>"
echo "</head>"
echo "<body>"
echo "<h1>Bienvenue sur ma page HTML!</h1>"
echo "<p>C'est une page HTML simple générée par un script shell.</p>"
echo "</body>"
echo "</html>"
