<?php
// Script PHP avec une boucle infinie

while (true) {
    // Votre code ici
    // Par exemple, vous pouvez ajouter un délai pour éviter d'utiliser trop de CPU
    echo "This is an infinite loop.\n";
    flush(); // Envoyer les données au navigateur
    sleep(1); // Délai d'une seconde
}
?>
