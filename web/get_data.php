<?php
require("config.php");
// weight is set to 99999 if transmitter detects a weight spike
// where id%30=0
$sql_srtr = "select *  from LoRaLog  where reading_time between date_add(NOW(), INTERVAL -1 day) and NOW() order by id desc";
$get_data = $conn->prepare($sql_srtr);
$get_data->execute();
if($get_data->rowCount()>0)
{
 while($value = $get_data->fetch(PDO::FETCH_OBJ))
 {
   $temperature = $value->temperature;
   $light = $value->light;
   $humidity = $value->humidity;
   $weight = $value->weight;
   $pressure = $value->pressure;
   $sequence = $value->sequence;
   $reading_time = $value->reading_time;
   $battery = $value->battery;
   $rssi = $value->rssi;
   $lostpackets = $value->lostpackets;
   $quality = $value->quality;
   $CO2 = $value->CO2;
   $VOC = $value->VOC;
   $result_array[] = ['temperature'=>$temperature, 'light'=>$light,
        'humidity'=>$humidity, 'weight'=>$weight,
        'pressure'=>$pressure, 'sequence'=>$sequence,
        'reading_time'=>$reading_time, 'battery'=>$battery,
        'rssi'=>$rssi,'lostpackets'=>$lostpackets,'quality'=>$quality,
        'CO2'=>$CO2,'VOC'=>$VOC];
 }
 echo json_encode($result_array);
 die();
}
?> 