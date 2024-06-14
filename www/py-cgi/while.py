#!/usr/bin/env python3

import time
import sys

# En-tête HTTP pour indiquer que le contenu est du texte brut
print("Content-Type: text/plain\r")
print("\r")

# Boucle de 10 secondes
start_time = time.time()
while time.time() - start_time < 10:
    print("Boucle en cours")
    sys.stdout.flush()  # Assurer que la sortie est envoyée immédiatement
    time.sleep(1)  # Attendre 1 seconde avant de passer à l'itération suivante
print("Boucle terminée")
