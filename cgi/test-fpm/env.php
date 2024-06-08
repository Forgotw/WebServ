<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Show env</title>
</head>
<body>
	<h1>Env</h1>
	<?php
	if (!empty($_SERVER)) {
	?>
	<h2>SERVER</h2>
	<ul>
	<?php
	foreach ($_SERVER as $key => $value) {
	?>
		<li><strong><?= $key ?></strong>: <?= $value ?></li>
	<?php
	}
	?>
	</ul>
	<hr>
	<?php
	}
	?>
	<?php
	if (!empty($_ENV)) {
	?>
	<h2>ENV</h2>
	<ul>
	<?php
	foreach ($_ENV as $key => $value) {
	?>
		<li><strong><?= $key ?></strong>: <?= $value ?></li>
	<?php
	}
	?>
	</ul>
	<hr>
	<?php
	}
	?>
	<?php
	if (!empty($_REQUEST)) {
	?>
	<h2>REQUEST</h2>
	<ul>
	<?php
	foreach ($_REQUEST as $key => $value) {
	?>
		<li><strong><?= $key ?></strong>: <?= $value ?></li>
	<?php
	}
	?>
	</ul>
	<?php
	}
	?>
</body>
</html>
