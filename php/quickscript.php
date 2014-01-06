<?php
/*
==========================================================
Filename:
---------
quickscript.php

Description:
------------
This PHP file handles insertion of data into the bigdata_data table.

Nikhil Venkatesh
01/04/2014
==========================================================
*/

// Connect to the database 
$mysqli = new mysqli("db9.cs.pitt.edu", "nikhil", "111213", "bigdata_data");
if ($mysqli->connect_error){
	$error_msg = "Could not connect to bigdata_data " . $mysqli->connect_errno . " : " . $mysqli->connect_error;
	die($error_msg);
}

// Create a file pointer for the file to read in
if (($fp = fopen("converted.csv", "r")) !== FALSE) {
	while (($data = fgetcsv($fp, 1000, ",")) !== FALSE) {
		$comma_separated = implode(", ", $data); 
		echo $comma_separated . "\n";
	}
	fclose($fp);
}

$mysqli->close();

?>