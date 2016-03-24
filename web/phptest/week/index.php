<?php
//header("refresh: 3;");
header("Content-Type: text/html; charset=utf-8");
?>
<?php
$fp = fsockopen("xinu00.cs.purdue.edu", 49909, $errno, $errstr, 30);
if (!$fp) {
    echo "$errstr ($errno)<br />\n";
} else {
	$out = "Afeng8     /xxx     /yyy       $";
	$command = "";
	$command .= chr(0);
	$command .= chr(7);
	$command .= "MY_WEEK";
	/*echo $command;*/
	fwrite($fp, $out);
	fwrite($fp, $command);
	$result = fgets($fp, 512);
	//$result = "W+class1|M|14:30|50+class1|W|14:30|50+class1|F|14:30|50+class2|T|10:00|75+class2|R|10:00|75+$";
	$classarr;
	if($result{0}=='W'){
		$classarr=explode("+", substr($result, 2));
	}
	fclose($fp);
}
?>

<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>CS490 test</title>
<link rel="stylesheet" href="../css/index.css" type="text/css" media="screen" />
</head>
<body>
<div align="left" id="mainWrapper">
	<?php include_once("../template_header.php");?>
	<div id="pageContent">
	<div id="singlewrap">
	<a href="http://localhost/phptest/">BACK</a>
	<br>
	<br>
	<br>
	<?php 
	if($result != ''){
		if(strcmp($result,"NACK\n")==0){
			echo "SERVICE ERROR.";
		}
		else{
			echo "My Schedule: ";
			if(strcmp($classarr[count($classarr)-1],"$")==0){
				array_pop($classarr);
				echo "<table><tr><th>title</th><th>day</th><th>time</th><th>duration</th></tr>";
				foreach($classarr as $classstr){
					echo "<tr>";
					list($cname, $cday, $ctime, $cdur) = explode("|", $classstr);
					echo '<td>' . $cname . '</td>';
					echo '<td>' . $cday . '</td>';
					echo '<td>' . $ctime . '</td>';
					echo '<td>' . $cdur . '</td>';
					echo "</tr>";
				}
				echo "</table>";
			}
		} 
	}
	?>
	</div>
	</div>
	<?php include_once("../template_footer.php");?>
</div>
</body>
</html>