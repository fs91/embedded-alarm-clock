<?php
//header("refresh: 3;");
header("Content-Type: text/html; charset=utf-8");
?>
<?php
$sensor=$_GET['s'];
if($sensor == 'A'){
	$fp = fsockopen("xinu00.cs.purdue.edu", 49909, $errno, $errstr, 30);
	if (!$fp) {
    	echo "$errstr ($errno)<br />\n";
    } else {
	    $out = "Afeng8     /xxx     /yyy       $";
	   	$command = "";
	   	$command .= chr(0);
	   	$command .= chr(8);
	   	$command .= "SENSOR_A";
    	/*echo $command;*/
	   	fwrite($fp, $out);
	   	fwrite($fp, $command);
	   	$result = fgets($fp, 128);
    	fclose($fp);
    }
}
if($sensor == 'B'){
	$fp = fsockopen("xinu00.cs.purdue.edu", 49909, $errno, $errstr, 30);
	if (!$fp) {
    	echo "$errstr ($errno)<br />\n";
    } else {
	    $out = "Afeng8     /xxx     /yyy       $";
	   	$command = "";
	   	$command .= chr(0);
	   	$command .= chr(8);
	   	$command .= "SENSOR_B";
    	/*echo $command;*/
	   	fwrite($fp, $out);
	   	fwrite($fp, $command);
	   	$result = fgets($fp, 128);
    	fclose($fp);
    }
}
if($sensor == 'CL'){
	$fp = fsockopen("xinu00.cs.purdue.edu", 49909, $errno, $errstr, 30);
	if (!$fp) {
    	echo "$errstr ($errno)<br />\n";
    } else {
	    $out = "Afeng8     /xxx     /yyy       $";
	   	$command = "";
	   	$command .= chr(0);
	   	$command .= chr(5);
	   	$command .= "CLOSE";
    	/*echo $command;*/
	   	fwrite($fp, $out);
	   	fwrite($fp, $command);
	   	$result = fgets($fp, 128);
    	fclose($fp);
    }
}
?>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>CS490 test</title>
<link rel="stylesheet" href="css/index.css?1" type="text/css" media="screen" />
</head>
<body>
<div align="left" id="mainWrapper">
	<?php include_once("template_header.php");?>
	<div id="pageContent">
	<div id="singlewrap">
	<a href="http://localhost/phptest/?s=A">SENSOR_A</a>
	<br>
	<a href="http://localhost/phptest/?s=B">SENSOR_B</a>
	<br>
	<a href="http://localhost/phptest/week/">MY_WEEK</a>
	<br>
	<a href="http://localhost/phptest/?s=CL">CLOSE</a>
	<br>
	<br>
	<?php 
	if($result != ''){
		echo "From pi: ";
		echo $result; 
	}
	?>
	</div>
	</div>
	<?php include_once("template_footer.php");?>
</div>
</body>
</html>