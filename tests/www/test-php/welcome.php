<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Bienvenue</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
            background-color: #f4f4f4;
        }
        .container {
            max-width: 400px;
            margin: 50px auto;
            padding: 20px;
            background-color: #fff;
            border-radius: 5px;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
            text-align: center;
        }
    </style>
</head>
<body>

<div class="container">
    <h2>Bienvenue</h2>
    <p>Vous êtes connecté avec succès en tant que <?php echo isset($_POST["username"]) ? $_POST["username"] : "Utilisateur"; ?>.</p>
    <p><a href="logout.php">Déconnexion</a></p>
</div>

</body>
</html>
