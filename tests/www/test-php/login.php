<?php
// Vérifier si le formulaire a été soumis
if ($_SERVER["REQUEST_METHOD"] == "POST") {
    // Récupérer les données du formulaire
    $username = $_POST["username"];
    $password = $_POST["password"];

    // Simuler une vérification de l'authentification
    // En pratique, vous devriez comparer les données avec une base de données ou un autre système d'authentification sécurisé
    // Ici, nous utilisons simplement des valeurs statiques à des fins de démonstration
    $valid_username = "utilisateur";
    $valid_password = "motdepasse";

    // Vérifier si les identifiants sont valides
    if ($username === $valid_username && $password === $valid_password) {
        // Redirection vers une page de succès
        header("Location: welcome.php");
        exit;
    } else {
        // Redirection vers une page d'erreur d'authentification
        header("Location: error.php");
        exit;
    }
}
?>
