<?php
/*
  Copyright Stuart A Spray  gitHub 'simplyEngineering'
  Thanks to Rui Santos for some elements - see https://RandomNerdTutorials.com
*/

$servername = "localhost";

// REPLACE with your Database name
$dbname = "cl55-acresaver";
// REPLACE with Database user
$username = "cl55-acresaver";
// REPLACE with Database user password
$password = "A6xbw8Lyszd7.";

// Keep this API Key value to be compatible with the ESP32 code provided in the project page. If you change this value, the ESP32 sketch needs to match
$api_key_value = "tPmAT5Ab3j7F9";
$err= "";
$err1= "";
$api_key = $temperature= $pressure= $humidity= $light= $weight= $battery= $sequence= $rxreset= $packet= $postcount= $rssi= $lostpackets= $quality= $CO2= $VOC="";

    $api_key = test_input($_POST["api_key"]);
    if($api_key == $api_key_value) 
    {
        $temperature = test_input($_POST["temperature"]);
        $pressure= test_input($_POST["pressure"]);
        $humidity= test_input($_POST["humidity"]);
        $light= test_input($_POST["light"]);
        $weight= test_input($_POST["weight"]);
        $battery= test_input($_POST["battery"]);
        $sequence= test_input($_POST["sequence"]);
        $rxreset= test_input($_POST["rxreset"]);
        $packet= test_input($_POST["packet"]);
        $postcount= test_input($_POST["postcount"]);
        $rssi= test_input($_POST["rssi"]);
		$lostpackets= test_input($_POST["lostpackets"]);
        $quality= test_input($_POST["quality"]);
        $CO2= test_input($_POST["CO2"]);
        $VOC= test_input($_POST["VOC"]);

        // Create connection
        // Check connection
        echo "Hi";
        $conn = mysqli_connect($servername,$username,$password,$dbname);
        if ($conn->connect_error)
        {
            $err= "Connect failure";
            die("Connection failed: " . $conn->connect_error);
        } 
        else $err= "Connect success";
        $sql = "INSERT INTO LoRaLog(temperature, pressure, humidity,light,weight,battery,sequence,rxreset,packet,postcount,rssi,lostpackets,quality,CO2,VOC)
        VALUES ('" . $temperature. "', '" . $pressure. "', '" . $humidity. "','" . $light. "', '" . $weight. "', '" . $battery.  "','" . $sequence. "','" . $rxreset. "','" . $packet. "','" . $postcount. "','" . $rssi. "','" . $lostpackets. "','" . $quality."','" . $CO2."','" . $VOC."')";
        
        echo $sql;
        echo $err;
        if ($conn->query($sql) === TRUE) 
        {
            $err1 = "record added";
            echo "New record created successfully";
        } 
        else 
        {
            echo "Error: " . $sql . "<br>" . $conn->error;
            $err1 = "record NOT added";
        }
        $conn->close();
        echo "err1";
    }
    else 
    {
        echo $sql;
        echo "Wrong API Key provided.";
    }
function test_input($data) 
{
    $data = trim($data);
    $data = stripslashes($data);
    $data = htmlspecialchars($data);
    return $data;
}
?>
