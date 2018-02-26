<?php

$username = "mhix10ho_ken777";
$password = "1235878952";
$database = "mhix10ho_test";
$tablename = "test";
$localhost = "xo2.x10hosting.com";
$con = mysql_connect($localhost, $username, $password);
$condb = mysql_select_db($database);

if ($con){
	$humidity = $_GET["humidity"];
	$temp = $_GET["temp"];
	if($condb){
	$query = "INSERT INTO `test` (`Humidity`, `Temperature`) VALUES ('".$humidity."','".$temp."');";
	//$query = "INSERT INTO `test` (`Humidity`, `Temperature`) VALUES ('59','77');";
	echo(' connect to database.');
	mysql_query($query);
	echo $humidity;
	echo $temp;
  } else {
  	echo('Unable to connect to database.');
  }
  
?>