<?php
// Chemin vers le fichier que vous souhaitez retourner
$chemin_fichier = './file.php';

// Vérifie si le fichier existe
if (file_exists($chemin_fichier)) {
    // Définit les en-têtes pour indiquer au navigateur qu'il s'agit d'un fichier à télécharger
    header('Content-Description: File Transfer');
    header('Content-Type: application/octet-stream');
    header('Content-Disposition: attachment; filename="' . basename($chemin_fichier) . '"');
    header('Content-Transfer-Encoding: binary');
    header('Expires: 0');
    header('Cache-Control: must-revalidate');
    header('Pragma: public');
    header('Content-Length: ' . filesize($chemin_fichier));

    // Lit le fichier et le retourne
    readfile($chemin_fichier);
    exit;
} else {
    // Si le fichier n'existe pas, vous pouvez afficher un message d'erreur ou rediriger l'utilisateur
    echo "Le fichier demandé n'existe pas.";
}
?>