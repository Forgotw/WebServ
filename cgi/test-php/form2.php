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
	if(!empty($_GET)) {
	?>
	<h2>New GET datas</h2>
	<ul>
		<li><strong>First name</strong>: <?= $_GET['firstname'] ?></li>
		<li><strong>Last name</strong>: <?= $_GET['lastname'] ?></li>
	</ul>
	<?php
	} else if(!empty($_POST)) {
	?>
	<h2>New POST datas</h2>
	<ul>
		<li><strong>First name</strong>: <?= $_POST['firstname'] ?></li>
		<li><strong>Last name</strong>: <?= $_POST['lastname'] ?></li>
	</ul>
	<?php
	} else {
	?>
		<h2>No data</h2>
	<?php
	}
	?>
</body>
</html>