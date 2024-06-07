<?php
	if ($_SERVER['REQUEST_METHOD'] === 'DELETE') {
		if (isset($_GET['name'])) {
			$filename = 'uploads/' . $_GET['name'];
			if (is_file($filename)) {
				if (unlink($filename)) {
					echo "File " . $_GET['name'] . " has been deleted\n";
				} else {
					echo "An error occured while trying to delete the file " . $_GET['name'] . "\n";
				}
			} else {
				echo "File " . $_GET['name'] . " does not exist\n";
			}
		} else {
			echo ("no name specified\n");
			echo ("Listing files...\n");
			$files = scandir('uploads');
			foreach ($files as $file) {
				if (is_file('uploads/' . $file)) {
					echo $file . "\n";
				}
			}
			echo "\nTo delete a file, add ?name=<file_name> to the end of the URL\n";
		}
	} else {
		echo "Method is not DELETE\n";
	}
?>
