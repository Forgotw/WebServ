<?php
// Démarrer la session pour pouvoir utiliser les variables de session si nécessaire
session_start();

// Définir le répertoire de téléchargement
$uploadDir = getenv('UPLOAD_DIR');
if ($uploadDir === false) {
    die('UPLOAD_DIR environment variable is not set.');
}

// Vérifiez si le formulaire de téléchargement a été soumis
if ($_SERVER['REQUEST_METHOD'] == 'POST' && isset($_FILES['fileToUpload'])) {
    $targetFile = $uploadDir . basename($_FILES['fileToUpload']['name']);
    $uploadOk = 1;
    $fileType = strtolower(pathinfo($targetFile, PATHINFO_EXTENSION));

    // Vérifier si le fichier est une image (facultatif)
    if (isset($_POST["submit"])) {
        $check = getimagesize($_FILES['fileToUpload']['tmp_name']);
        if ($check !== false) {
            echo "File is an image - " . $check['mime'] . ".<br>";
            $uploadOk = 1;
        } else {
            echo "File is not an image.<br>";
            $uploadOk = 0;
        }
    }

    // Vérifiez si le fichier existe déjà
    if (file_exists($targetFile)) {
        echo "Sorry, file already exists.<br>";
        $uploadOk = 0;
    }

    // Vérifiez la taille du fichier (facultatif, ici limité à 5MB)
    if ($_FILES['fileToUpload']['size'] > 5000000) {
        echo "Sorry, your file is too large.<br>";
        $uploadOk = 0;
    }

    // Autoriser certains formats de fichiers (facultatif)
    $allowedTypes = ['jpg', 'png', 'jpeg', 'gif'];
    if (!in_array($fileType, $allowedTypes)) {
        echo "Sorry, only JPG, JPEG, PNG & GIF files are allowed.<br>";
        $uploadOk = 0;
    }

    // Vérifiez si $uploadOk est défini à 0 par une erreur
    if ($uploadOk == 0) {
        echo "Sorry, your file was not uploaded.<br>";
    } else {
        // Si tout est ok, essayez de télécharger le fichier
        if (move_uploaded_file($_FILES['fileToUpload']['tmp_name'], $targetFile)) {
            echo "The file " . basename($_FILES['fileToUpload']['name']) . " has been uploaded.<br>";
        } else {
            echo "Sorry, there was an error uploading your file.<br>";
            // Afficher des informations d'erreur détaillées
            echo "Error Code: " . $_FILES['fileToUpload']['error'] . "<br>";
            switch ($_FILES['fileToUpload']['error']) {
                case UPLOAD_ERR_INI_SIZE:
                    echo "The uploaded file exceeds the upload_max_filesize directive in php.ini.<br>";
                    break;
                case UPLOAD_ERR_FORM_SIZE:
                    echo "The uploaded file exceeds the MAX_FILE_SIZE directive that was specified in the HTML form.<br>";
                    break;
                case UPLOAD_ERR_PARTIAL:
                    echo "The uploaded file was only partially uploaded.<br>";
                    break;
                case UPLOAD_ERR_NO_FILE:
                    echo "No file was uploaded.<br>";
                    break;
                case UPLOAD_ERR_NO_TMP_DIR:
                    echo "Missing a temporary folder.<br>";
                    break;
                case UPLOAD_ERR_CANT_WRITE:
                    echo "Failed to write file to disk.<br>";
                    break;
                case UPLOAD_ERR_EXTENSION:
                    echo "A PHP extension stopped the file upload.<br>";
                    break;
                default:
                    echo "Unknown upload error.<br>";
                    break;
            }
        }
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Upload File</title>
</head>
<body>
    <form action="upload.php" method="post" enctype="multipart/form-data">
        Select file to upload:
        <input type="file" name="fileToUpload" id="fileToUpload">
        <input type="submit" value="Upload File" name="submit">
    </form>
</body>
</html>
