<?php
if($_SERVER['REQUEST_METHOD'] == 'POST') {
	if (isset($_FILES['file']) && $_FILES['file']['error'] == 0) {
		$uploadDir = "uploads";
		$fileExtension = pathinfo($_FILES['file']['name'], PATHINFO_EXTENSION);
		$fileName = uniqid() . '.' . $fileExtension;
		$filePath = $uploadDir . '/' . $fileName;
		if (move_uploaded_file($_FILES['file']['tmp_name'], $filePath)) {
			header('Location: upload.php?path=' . $filePath);
		} else {
			header('Location: upload.php?error=move');
		}

	} else {
		header('Location: upload.php?error=upload');
		exit;
	}
}
?>
