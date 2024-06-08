<?php
// Activer l'affichage des erreurs pour le débogage
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

// Définir le répertoire de téléchargement
$upload_dir = getenv('TMPDIR') ?: '/tmp';

// Vérifier si le répertoire de téléchargement existe
if (!file_exists($upload_dir)) {
    try {
        // Créer le répertoire
        mkdir($upload_dir, 0777, true);
        // Définir les permissions
        chmod($upload_dir, 0777);
    } catch (Exception $e) {
        header("HTTP/1.1 500 Internal Server Error");
        echo "<html><body><h1>Internal Server Error</h1><p>{$e->getMessage()}</p></body></html>";
        exit(1);
    }
}

function main() {
    global $upload_dir;

    // Vérifier la taille du contenu
    if (empty($_SERVER['CONTENT_LENGTH'])) {
        header("HTTP/1.1 400 Bad Request");
        echo "<html><body><h1>Bad Request: No content received</h1></body></html>";
        return;
    }

    // Vérifier si un fichier a été téléchargé
    if (!isset($_FILES['file'])) {
        header("HTTP/1.1 400 Bad Request");
        echo "<html><body><h1>Bad Request: No file field in form</h1></body></html>";
        return;
    }

    $fileitem = $_FILES['file'];

    // Vérifier s'il y a une erreur lors du téléchargement
    if ($fileitem['error'] !== UPLOAD_ERR_OK) {
        header("HTTP/1.1 400 Bad Request");
        echo "<html><body><h1>Bad Request: File upload error</h1></body></html>";
        return;
    }

    // Vérifier si un fichier a été téléchargé
    if (!empty($fileitem['name'])) {
        // Nettoyer le nom du fichier pour éviter les problèmes de sécurité
        $filename = basename($fileitem['name']);
        $filepath = $upload_dir . DIRECTORY_SEPARATOR . $filename;

        // Si le fichier existe déjà, ajouter "_copy" au nom
        if (file_exists($filepath)) {
            $base = pathinfo($filename, PATHINFO_FILENAME);
            $extension = pathinfo($filename, PATHINFO_EXTENSION);
            $copy_number = 1;
            $new_filename = "{$base}_copy.{$extension}";
            $new_filepath = $upload_dir . DIRECTORY_SEPARATOR . $new_filename;
            while (file_exists($new_filepath)) {
                $copy_number++;
                $new_filename = "{$base}_copy{$copy_number}.{$extension}";
                $new_filepath = $upload_dir . DIRECTORY_SEPARATOR . $new_filename;
            }
            $filename = $new_filename;
            $filepath = $new_filepath;
        }

        // Écrire le fichier sur le disque
        if (move_uploaded_file($fileitem['tmp_name'], $filepath)) {
            // Répondre au client
            header("HTTP/1.1 201 Created");
            echo "<html><body><h1>File '{$filename}' uploaded successfully</h1></body></html>";
        } else {
            header("HTTP/1.1 500 Internal Server Error");
            echo "<html><body><h1>Internal Server Error: Unable to move uploaded file</h1></body></html>";
        }
    } else {
        header("HTTP/1.1 400 Bad Request");
        echo "<html><body><h1>Bad Request: No file uploaded</h1></body></html>";
    }
}

main();
?>
