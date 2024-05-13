<?php
// Vérifie si des données ont été envoyées
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Vérifie si le fichier a été correctement uploadé sans erreur
    if (isset($_FILES["image"]) && $_FILES["image"]["error"] == UPLOAD_ERR_OK) {
        // Chemin où vous souhaitez sauvegarder le fichier
        $upload_dir = "uploads/";
        
        // Vérifie si le dossier d'upload existe, sinon le crée
        if (!file_exists($upload_dir)) {
            mkdir($upload_dir, 0777, true);
        }
        
        // Chemin complet du fichier uploadé
        $upload_file = $upload_dir . basename($_FILES["image"]["name"]);
        
        // Déplace le fichier uploadé vers le dossier d'upload spécifié
        if (move_uploaded_file($_FILES["image"]["tmp_name"], $upload_file)) {
            echo "L'image a été correctement uploadée.";
        } else {
            echo "Une erreur est survenue lors de l'upload de l'image.";
        }
    } else {
        echo "Erreur: Aucun fichier uploadé ou une erreur est survenue.";
    }
} else {
    echo "Erreur: Cette page ne peut être accédée directement.";
}
?>
