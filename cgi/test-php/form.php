<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Formulaires</title>
</head>
<body>
    <h1>Formulaires</h1>
    <?php
    error_reporting(E_ALL); // Activer les rapports d'erreurs
    ini_set('display_errors', 1); // Afficher les erreurs

    if (!empty($_GET['firstname']) && !empty($_GET['lastname'])) {
    ?>
    <h2>New GET datas</h2>
    <ul>
        <li><strong>First name</strong>: <?= htmlspecialchars($_GET['firstname']) ?></li>
        <li><strong>Last name</strong>: <?= htmlspecialchars($_GET['lastname']) ?></li>
    </ul>
    <?php
    } else if (!empty($_POST['firstname']) && !empty($_POST['lastname'])) {
    ?>
    <h2>New POST datas</h2>
    <ul>
        <li><strong>First name</strong>: <?= htmlspecialchars($_POST['firstname']) ?></li>
        <li><strong>Last name</strong>: <?= htmlspecialchars($_POST['lastname']) ?></li>
    </ul>
    <?php
    } else {
    ?>
    <h2>No data</h2>
    <?php
    }
    ?>

    <h2>Submit GET Data</h2>
    <form method="get" action="">
        <label for="firstname">First name:</label>
        <input type="text" id="firstname" name="firstname"><br>
        <label for="lastname">Last name:</label>
        <input type="text" id="lastname" name="lastname"><br>
        <input type="submit" value="Submit">
    </form>

    <h2>Submit POST Data</h2>
    <form method="post" action="">
        <label for="firstname_post">First name:</label>
        <input type="text" id="firstname_post" name="firstname"><br>
        <label for="lastname_post">Last name:</label>
        <input type="text" id="lastname_post" name="lastname"><br>
        <input type="submit" value="Submit">
    </form>
</body>
</html>
