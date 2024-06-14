<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Session ID Display</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f0f0;
        }
        .container {
            text-align: center;
            background-color: #fff;
            padding: 20px;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }
        .session-id {
            font-size: 24px;
            font-weight: bold;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="session-id">
            <?php
            // Récupérer la variable SESSIONID depuis les variables d'environnement
            $sessionID = getenv('SESSION_ID');
            if ($sessionID === false) {
                echo "SESSIONID not set";
            } else {
                echo "Your sessionID is: " . htmlspecialchars($sessionID);
            }
            ?>
        </div>
    </div>
</body>
</html>