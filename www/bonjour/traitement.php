<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<title>Traitement du formulaire</title>
	</head>
	<body>
<?php if (isset($_POST['firstname'], $_POST['lastname'])) { ?>
	<h2>New POST !!</h2>
	<p>
		firstname: <?php echo $_POST['firstname']; ?><br>
		lastname: <?php echo $_POST['lastname']; ?><br>
	</p>
<?php } else if (isset($_GET['firstname'], $_GET['lastname'])) { ?>
	<h2>New GET !!</h2>
	<p>
		firstname: <?php echo $_GET['firstname']; ?><br>
		lastname: <?php echo $_GET['lastname']; ?><br>
	</p>
<?php } else if (isset($_FILES['file'])) { ?>
	<h2>New file !!</h2>
	<p>
		File name: <?php echo $_FILES["file"]["name"]; ?><br>
		File full path: <?php echo $_FILES["file"]["full_path"]; ?><br>
		File type: <?php echo $_FILES["file"]["type"]; ?><br>
		File tmp name: <?php echo $_FILES["file"]["type"]; ?><br>
		File error: <?php $_FILES["file"]["error"]; ?><br>
		File size: <?php echo $_FILES["file"]["size"]; ?><br>
	</p>
<?php
	$targetDir = "./";
	$targetFile = $targetDir . basename($_FILES["file"]["name"]);
	if (move_uploaded_file($_FILES["file"]["tmp_name"], $targetFile)) {
		echo "The file has been uploaded";
	} else {
		echo "Sorry, there was an error uploading your file";
	}
} else { echo "No data"; }
?>
	</body>
</html>
