<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Upload</title>
</head>
<body>
	<h1>Upload</h1>
	<form action="upload_script.php" method="post" enctype="multipart/form-data">
		<input type="file" name="file" id="file">
		<input type="submit" value="Send">
	</form>

	<?php
	if (isset($_GET['error'])) {
	?>
	<h2>Error !!!</h2>
	<p>Error: <?= $_GET['error'] ?></p>
	<?php
	} else if (isset($_GET['path'])) {
	?>
	<h2>New link</h2>
	<p>
		New link: <a href="<?= $_GET['path'] ?>" target="_blank"><?= $_GET['path'] ?></a>
	</p>
	<?php
	}
	?>
</body>
</html>
