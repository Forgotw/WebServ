<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Traitement du formulaire</title>
</head>
<body>
    <h2>Réception des données du formulaire</h2>
    <?php
    // Vérifier si des données ont été envoyées via POST
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        // Imprimer le contenu de stdin dans la sortie d'erreur
        error_log("Contenu de stdin :\n" . file_get_contents("php://stdin"));
        // Afficher le contenu de stdin
        echo "<pre>";
        echo "Contenu de stdin :\n";
        echo file_get_contents("php://stdin");
        echo "</pre>";
        // Récupérer les données du formulaire
        $name = $_POST["name"];
        $email = $_POST["email"];

        // Afficher les données reçues
        echo "<p>Nom : " . $name . "</p>";
        echo "<p>Email : " . $email . "</p>";
    } else {
        // Si aucune donnée n'a été envoyée, afficher un message d'erreur
        echo "<p>Aucune donnée reçue.</p>";
    }
    ?>
</body>
</html>
